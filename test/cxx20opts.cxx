#include <cxx20opts/cxx20opts.hxx>
#include <concepts>
#include <memory>
#include <limits>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <iostream>
#include <limits.h>
#include <sstream>

using namespace cxx20opts;
using namespace cxx20opts::literals;

int main(int argc, char* argv[] /* this is vla? */) {
    cxx20opts::options opts{argc, argv};

    opts.add(option{.short_name = "h", .name = "help", .description = "Print help"});

    opts | option{.short_name = "f", .name = "file", .description = "Path to Some File"}  //
        | option{.name = "verbose", .description = "verbose output"}                      //
        | option{.short_name = "o", .name = "output", .description = "output file"}       //
        | option{.name = "version", .description = "show app version"};                   //

    opts | enable_windows_like_argument | disable_windows_like_argument |
        enable_windows_like_argument;
    opts.disable_windows_like_argument();

    opts | enable_help;
    opts.enable_help_output();


    // set argument for print help
    opts.custom_help_trigger({"MyHelp"});
    opts | help_argument{"SomeHelp"};
    opts | help_argument{"SomeHelp"};

    std::string h{"asdasds"};
    opts | help_argument{h};

    opts | disable_help;
    opts.disable_help_output();

    option x{.name = "none", .description = "NoNe"};
    //    x.defautl_value<std::string_view>(""); // not implemented.

    opts.add(x);             // copy
    opts.add(std::move(x));  // move

    opts(option{.name = "kek", .description = "lol"})({{}, {}, "*-_-*"});

    opts["kecasdak"];                          // unchecked UB, out of range, ubsan не ловит
    opts["v"], opts["version"];                // semantically equivalent
    opts.at("K"), opts.at("ADKFJDASKJFADHA");  // throws exception - out of range
    assert(opts.raw().argc == argc);           // get argc
    assert(opts.raw().argv == argv);           // get argv


    std::stringstream info;
    info << "(${commit_timestamp}, собрано " << __DATE__ << " " << __TIME__ << ")";

    opts.description(program_description{
        .name = executable_name(opts.raw()) + " - CXX 20 Opts Test Programm",  //
        .version = std::string{std::string{"v"} + std::to_string(0.42f)},      //
        .info = info.str()                                                     //
    });

    opts | enable_help;
    opts.print_help(std::cout);

    return 0;
}

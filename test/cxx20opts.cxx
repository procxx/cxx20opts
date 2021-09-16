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
#include <cassert>

using namespace cxx20opts;
using namespace cxx20opts::literals;

void test_add_options(cxx20opts::options& opts) {
    opts.add(option{.short_name = "h", .name = "help", .description = "Print help"});

    opts | option{.short_name = "f", .name = "file", .description = "Path to Some File"}  //
        | option{.name = "verbose", .description = "verbose output"}                      //
        | option{.short_name = "o", .name = "output", .description = "output file"}       //
        | option{.name = "version", .description = "show app version"};                   //


    option x{.name = "none", .description = "NoNe"};
    //    x.defautl_value<std::string_view>(""); // not implemented.

    opts.add(x);             // copy
    opts.add(std::move(x));  // move

    opts(option{.name = "kek", .description = "lol"})({{}, {}, "*-_-*"});
}

void test_windows_style_behavior(cxx20opts::options& opts) {
    opts | enable_windows_style_behavior | disable_windows_style_behavior |
        enable_windows_style_behavior;
    assert(opts.status_windows_style_behavior());
    opts.disable_windows_style_behavior();
    assert(not opts.status_windows_style_behavior());
}

void test_help(cxx20opts::options& opts) {
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


    opts | enable_help;
    opts.print_help(std::cout);
}

void test_get_options(const cxx20opts::options& opts, int argc, char* argv[]) {
    opts["kecasdak"];                          // unchecked UB, out of range, ubsan не ловит
    opts["v"], opts["version"];                // semantically equivalent
    opts.at("K"), opts.at("ADKFJDASKJFADHA");  // throws exception - out of range
    assert(opts.raw()->argc == argc);          // get argc
    assert(opts.raw()->argv == argv);          // get argv
}

void test_description(cxx20opts::options& opts) {
    std::stringstream info;
    info << "(${commit_timestamp}, собрано " << __DATE__ << " " << __TIME__ << ")";

    opts.description(program_description{
        .name = executable_name(opts.raw().value()).string() + " - CXX 20 Opts Test Programm",  //
        .version = std::string{std::string{"v"} + std::to_string(0.42f)},                       //
        .info = info.str()                                                                      //
    });
}


int main(int argc, char* argv[] /* this is vla? */) {
    cxx20opts::options opts{argc, argv};

    //    test_windows_style_behavior(opts);
    //    test_add_options(opts);
    //    test_get_options(opts, argc, argv);
    //    test_help(opts);

    return 0;
}

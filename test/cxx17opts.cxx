#include <cxx20opts/cxx20opts.hxx>
#include <cassert>
#include <iostream>

using namespace cxx20opts;
using namespace std::string_view_literals;

std::ostream& operator<<(std::ostream& os, const option& o) {
    os << "short_name: " << o.short_name.value_or(""sv)       //
       << "\nname: " << o.name.value_or(""sv)                 //
       << "\ndescription: " << o.description.value_or(""sv);  //
    return os;
}

struct custom_user_struct {
    std::string_view Some{};
};

inline auto operator|(options& obj, const custom_user_struct& o) noexcept -> options& {
    obj.add(option{std::nullopt, o.Some.data(), "Descriptions"});
    return obj;
}

void test_options() {
    option x{std::nullopt, "version", "show app version"};
    std::cout << x << std::endl;
}


int main(int argc, char* argv[] /*this is vla?*/) {
    cxx20opts::options opts{argc, argv};
    //    test_options();

    option x{};
    (void)(x);

    x | "a"_short_name | "ask"_name |
        "Before performing the action, display what will take place"_description;

    opts.add(option{"-h", "help", "Print help"});
    opts | x | option{"f", "file", "Path to Some File"}  //
        | option{{}, "verbose", "verbose output"}        //
        | option{"o", "output", "output file"}           //
        | option{{}, "version", "show app version"}      //
    ;

    opts | option{{"d"}, {"dd"}, {"Descriptions..."}};

    opts(option{{}, "kek", "lol"});
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


    opts | custom_user_struct{"AAAA"};

    opts["kecasdak"];                          // unchecked UB, out of range, ubsan не ловит
    opts["v"], opts["version"];                // semantically equivalent
    opts.at("K"), opts.at("ADKFJDASKJFADHA");  // throws exception - out of range
    assert(opts.raw().argc == argc);           // get argc
    assert(opts.raw().argv == argv);           // get argv


    std::stringstream info;
    info << "(${commit_timestamp}, собрано " << __DATE__ << " " << __TIME__ << ")";

    opts.description(program_description{
        executable_name(opts.raw()).string() + " - CXX 20 Opts Test Programm",  // name
        std::string{std::string{"v"} + std::to_string(0.42f)},                  // version
        info.str()                                                              // info
    });

    opts | enable_help;
    opts.print_help(std::cout);


    return 0;
}

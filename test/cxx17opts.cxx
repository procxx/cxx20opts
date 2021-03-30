#include <cxx20opts/cxx20opts.hxx>
#include <cassert>
#include <iostream>

using namespace cxx20opts;

std::ostream& operator<<(std::ostream& os, const option& o) {
    os << "short_name: " << o.short_name.value_or("")       //
       << "\nname: " << o.name.value_or("")                 //
       << "\ndescription: " << o.description.value_or("");  //
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
    test_options();

    opts.add(option{"-h", "help", "Print help"});
    opts | option{"f", "file", "Path to Some File"}  //
        | option{"verbose", "verbose output"}        //
        | option{"o", "output", "output file"}       //
        | option{"version", "show app version"};     //

    opts | option{{}, {""}, {}};

    opts(option{"", "kek", "lol"})({})({})({});  // упрлся

    opts | enable_help;
    opts.enable_help_output();

    // set argument for print help
    opts.custom_help({"MyHelp"});
    opts | help_argument{"SomeHelp"};
    opts | help_argument{"SomeHelp"};

    std::string h{"asdasds"};
    opts | help_argument{h};

    opts | disable_help;
    opts.disable_help_output();


    opts | custom_user_struct{"AAAAAAAAAAA"};

    opts["kecasdak"];                          // unchecked UB, out of range, ubsan не ловит
    opts["v"], opts["version"];                // semantically equivalent
    opts.at("K"), opts.at("ADKFJDASKJFADHA");  // throws exception - out of range
    assert(opts.raw().argc == argc);           // get argc
    assert(opts.raw().argv == argv);           // get argv

    return 0;
}

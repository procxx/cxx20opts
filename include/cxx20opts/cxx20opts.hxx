#ifndef CXX20OPTS_HXX_INCLUDE_FILE
#define CXX20OPTS_HXX_INCLUDE_FILE

//#ifdef __cpp_exceptions
//#define cxx20opts_has_exceptions
//#endif

#if __cplusplus > 201703L
#define cxx20opts_has_cxx20
#endif

#if defined(cxx20opts_has_cxx20) && defined(__cpp_concepts)
#define cxx20opts_has_concepts
#endif

#if __has_attribute(pure)
#define cxx20opts_pure __attribute__((pure))
#else
#define cxx20opts_pure
#endif

#if __has_attribute(const)
#define cxx20opts_const __attribute__((const))
#else
#define cxx20opts_const
#endif


#include <memory>
#include <string_view>
#include <string>
#include <optional>
#include <unordered_set>
#include <stdexcept>
#include <ostream>
#include <vector>
#include <iostream>
#include <filesystem>



namespace cxx20opts {

    namespace fs = std::filesystem;


    namespace separator {
        using namespace std::string_view_literals;
        [[maybe_unused]] constexpr inline std::string_view short_ = "-"sv;
        [[maybe_unused]] constexpr inline std::string_view double_ = "--"sv;
        [[maybe_unused]] constexpr inline std::string_view windows_ = "/"sv;
    }  // namespace separator


    /// def impl, in development
    struct option {
        std::optional<std::string_view> short_name{std::nullopt};
        std::optional<std::string_view> name{std::nullopt};
        std::optional<std::string_view> description{std::nullopt};
    };

    struct name_t {
        std::string_view name{};
    };
    struct short_name_t {
        std::string_view short_name{};
    };
    struct description_t {
        std::string_view description{};
    };

    inline namespace literals {

        auto operator"" _name(const char* s) noexcept -> name_t { return {s}; }
        auto operator"" _short_name(const char* s) noexcept -> short_name_t { return {s}; }
        auto operator"" _description(const char* s) noexcept -> description_t { return {s}; }

        auto operator"" _name(const char* s, std::size_t n) noexcept -> name_t { return {{s, n}}; }
        auto operator"" _short_name(const char* s, std::size_t n) noexcept -> short_name_t {
            return {{s, n}};
        }
        auto operator"" _description(const char* s, std::size_t n) noexcept -> description_t {
            return {{s, n}};
        }

    }  // namespace literals

    inline auto operator|(option& z, name_t&& o) noexcept -> option& {
        z.name = o.name;
        return z;
    }
    inline auto operator|(option& z, short_name_t&& o) noexcept -> option& {
        z.short_name = o.short_name;
        return z;
    }
    inline auto operator|(option& z, description_t&& o) noexcept -> option& {
        z.description = o.description;
        return z;
    }


    struct raw_args {
        using count_t = int;
        using c_array_string_t = char**;

        const count_t argc;
        const c_array_string_t argv;
    };

    struct program_description {
        std::string name;
        std::optional<std::string> version;
        std::optional<std::string> info;
    };

    struct help_argument {
        std::string_view argument{/*"help"*/};
    };



    inline namespace tags {

        struct enable_windows_style_behavior_t {};
        struct disable_windows_style_behavior_t {};

        struct enable_help_t {};
        struct disable_help_t {};

        [[maybe_unused]] constexpr inline enable_help_t enable_help{};
        [[maybe_unused]] constexpr inline disable_help_t disable_help{};

        [[maybe_unused]] constexpr inline enable_windows_style_behavior_t
            enable_windows_style_behavior{};
        [[maybe_unused]] constexpr inline disable_windows_style_behavior_t
            disable_windows_style_behavior{};

        [[maybe_unused]] constexpr inline help_argument help_default{"help"};

    }  // namespace tags


    template <class T>
    struct value {};


    class options {
    public:
        constexpr static inline std::size_t number_path_to_exe_in_argv = 0;

        /* move this types to template args? Maybe later... */
        using raw_args_t = std::optional<raw_args>;
        using count_t = raw_args::count_t;
        using c_array_string_t = raw_args::c_array_string_t;

        using help_argument_t = help_argument;
        using program_description_t = std::optional<program_description>;
        using opts_t = std::optional<std::vector<option>>;


        constexpr options() noexcept {}
        constexpr options(program_description_t&& name) noexcept : description_{std::move(name)} {}
        options(const count_t argc_, char** argv_) noexcept {
            raw_.emplace(raw_args{argc_, argv_});
            parse_impl();
        }

        [[maybe_unused]] auto parse(const count_t argc_, char** argv_) noexcept -> options&;

        /* throws std::bad_alloc */
        [[maybe_unused]] auto operator()(const option&) -> options&;

        [[maybe_unused]] auto operator()(option&&) noexcept -> options&;

        /* throws std::bad_alloc */
        [[maybe_unused]] auto add(const option&) -> options&;

        [[maybe_unused]] auto add(option&&) noexcept -> options&;


        auto operator[](const count_t) const noexcept -> std::string_view;
        auto operator[](std::string_view) const noexcept -> std::string_view;

        /* throws std::out_of_range */
        auto at(const count_t) const -> std::string_view;
        /* throws std::out_of_range */
        auto at(std::string_view) const -> std::string_view;

        cxx20opts_pure auto raw() const noexcept -> const raw_args_t&;
        cxx20opts_pure auto opts() const noexcept -> const opts_t;

        // tags:

        [[maybe_unused]] auto enable_windows_style_behavior() noexcept -> options&;
        [[maybe_unused]] auto disable_windows_style_behavior() noexcept -> options&;
        [[maybe_unused]] auto status_windows_style_behavior() noexcept -> bool {
            return windows_style_behavior_on;
        }

        [[maybe_unused]] auto enable_help_output() noexcept -> options&;
        [[maybe_unused]] auto disable_help_output() noexcept -> options&;

        [[maybe_unused]] auto custom_help_trigger(help_argument_t&& h) noexcept -> options&;
        [[maybe_unused]] auto print_help(std::ostream& os) noexcept -> options&;

        [[maybe_unused]] auto description(program_description_t&& desc) noexcept -> options&;

        template <class T>
        [[maybe_unused]] auto operator()(option&&, value<T> t) noexcept -> options& {}

    private:
        // false by default
        bool windows_style_behavior_on{false};

        // true by default
        bool help_support{true};

        // "-help or --help" print help by default
        help_argument_t help_string_output{help_default};

        // pointers to argc && argv
        raw_args_t raw_{std::nullopt};

        // dyn array options.
        opts_t opts_{std::nullopt};


        // Program description, first line in help output, space separeted.
        // ${name} ${version} ${other_info}
        // example:
        //
        // VIM - Vi IMproved 8.2 (2019 Dec 12, собрано Mar 29 2021 17:00:35)
        //
        // name = "VIM - Vi IMproved"
        // version = "8.2"
        // info = "(2019 Dec 12, собрано Mar 29 2021 17:00:35)"

        program_description_t description_{std::nullopt};


        auto parse_impl() noexcept -> void;


#if defined(cxx20opts_has_concepts)
        //  cxx20opts_pure auto operator[](concepts::string_observer auto str) const noexcept ->
        //  std::string_view;
#endif
    };


    [[maybe_unused]] cxx20opts_const inline auto executable_name(const raw_args& args) noexcept
        -> fs::path {
        return fs::path{args.argv[options::number_path_to_exe_in_argv]}.filename();
    }

}  // namespace cxx20opts




namespace cxx20opts {

    inline auto options::parse(const options::count_t argc_, char** argv_) noexcept -> options& {
        raw_.emplace(raw_args{argc_, argv_});
        parse_impl();
        return *this;
    }

    inline auto options::add(option&& o) noexcept -> options& {
        opts_->emplace_back(std::move(o));
        return *this;
    }

    // a copy will be created option  /* throws std::bad_alloc */
    inline auto options::add(const option& o) -> options& {
        opts_->push_back(o);
        return *this;
    }


    // unchecked method
    inline auto options::operator[](const options::count_t n) const noexcept -> std::string_view {
        return {raw_->argv[n]};
    }

    // unchecked method
    inline auto options::operator[](std::string_view str) const noexcept -> std::string_view {
        // TODO: unimplemented

        (void)(str);
        return {"unimplemented"};
    }

    /* throws std::out_of_range */
    inline auto options::at(const options::count_t n) const -> std::string_view {
        if (not raw_.has_value()) {
            throw std::bad_optional_access{};
        }
        if (n > raw_->argc) {
            throw std::out_of_range{""};
        }
        return {raw_->argv[n]};
    }

    /* throws std::out_of_range */
    inline auto options::at(std::string_view str) const -> std::string_view {
        // TODO: unimplemented

        // throw std::out_of_range("");

        (void)(str);
        return {"unimplemented"};
    }

    inline auto options::raw() const noexcept -> const raw_args_t& { return raw_; }


    inline auto options::parse_impl() noexcept -> void {
        // TODO: unimplemented
        if (help_support) {
        }

        for (options::count_t i = 0; i < raw_->argc; ++i) {
            ;
        }
        if (windows_style_behavior_on) {
        }
    }


    inline auto options::custom_help_trigger(help_argument_t&& h) noexcept -> options& {
        help_string_output = std::move(h);
        return *this;
    }


    inline auto options::print_help(std::ostream& os = std::cout) noexcept -> options& {
        // indent three spaces
        constexpr static std::string_view padding = "   ";

        if (description_) {
            os << description_->name << " ";

            if (description_->version) {
                os << *description_->version << " ";
            }  // end if

            if (description_->info) {
                os << *description_->info << "\n";
            }  // end if

        }  // end if has description

        for (const auto& i : *opts_) {
            os << padding;

            if (i.short_name) {
                os << separator::short_;
                os << *i.short_name;
            }  // end if

            if (i.short_name && i.name) {
                os << " ";
            }  // end if

            if (i.name) {
                os << separator::double_ << *i.name;
            }  // end if

            if (i.description) {
                os << "\t\t" << *i.description;
            }  // end if

            os << "\n";
        }  // end for


        return *this;
    }

    inline auto options::description(program_description_t&& desc) noexcept -> options& {
        description_ = std::move(desc);
        return *this;
    }


    // more boilerplate...


    inline auto options::enable_windows_style_behavior() noexcept -> options& {
        windows_style_behavior_on = true;
        return *this;
    }
    inline auto options::disable_windows_style_behavior() noexcept -> options& {
        windows_style_behavior_on = false;
        return *this;
    }

    inline auto options::enable_help_output() noexcept -> options& {
        help_support = true;
        return *this;
    }
    inline auto options::disable_help_output() noexcept -> options& {
        help_support = false;
        return *this;
    }

    inline auto operator|(options& z, options::program_description_t&& desc) noexcept -> options& {
        z.description(std::move(desc));
        return z;
    }

    inline auto operator|(options& z, options::help_argument_t&& h) noexcept -> options& {
        z.custom_help_trigger(std::move(h));
        return z;
    }

    inline auto operator|(options& z, option&& o) noexcept -> options& {
        z.add(std::move(o));
        return z;
    }
    // a copy will be created option  /* throws std::bad_alloc */
    inline auto operator|(options& z, const option& o) -> options& {
        z.add(o);
        return z;
    }

    inline auto options::operator()(option&& o) noexcept -> options& {
        add(std::move(o));
        return *this;
    }
    // a copy will be created option  /* throws std::bad_alloc */
    inline auto options::operator()(const option& o) -> options& {
        add(o);
        return *this;
    }

    inline auto operator|(options& z, enable_windows_style_behavior_t&&) noexcept -> options& {
        z.enable_windows_style_behavior();
        return z;
    }
    inline auto operator|(options& z, const enable_windows_style_behavior_t&) noexcept -> options& {
        z.enable_windows_style_behavior();
        return z;
    }
    inline auto operator|(options& z, disable_windows_style_behavior_t&&) noexcept -> options& {
        z.disable_windows_style_behavior();
        return z;
    }
    inline auto operator|(options& z, const disable_windows_style_behavior_t&) noexcept
        -> options& {
        z.disable_windows_style_behavior();
        return z;
    }

    inline auto operator|(options& z, enable_help_t&&) noexcept -> options& {
        z.enable_help_output();
        return z;
    }
    inline auto operator|(options& z, const enable_help_t&) noexcept -> options& {
        z.enable_help_output();
        return z;
    }
    inline auto operator|(options& z, disable_help_t&&) noexcept -> options& {
        z.disable_help_output();
        return z;
    }
    inline auto operator|(options& z, const disable_help_t&) noexcept -> options& {
        z.disable_help_output();
        return z;
    }


}  // namespace cxx20opts

#endif

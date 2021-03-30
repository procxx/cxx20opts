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

#include <memory>
#include <string_view>
#include <string>
#include <optional>
#include <unordered_set>
#include <stdexcept>
#include <ostream>
#include <vector>

#ifdef cxx20opts_has_concepts

#if __has_include(<concepts>)
#include <concepts>
#endif

// TODO: unimplemented
namespace cxx20opts::concepts {

    template <class T>
    concept nothrow_default_ctorble =
        std::is_trivially_default_constructible_v<T> || std::is_nothrow_constructible_v<T>;

    template <class T>
    concept default_constructible = std::is_default_constructible_v<T>;

    template <class T>
    concept trivially_destructible = std::is_trivially_destructible_v<T>;

    template <class T>
    concept c_str = requires(T string) {
        { string.c_str() }  // need get c-style string
        noexcept->std::same_as<typename T::const_pointer>;
    };

    template <class T>
    concept old_const_iterators = requires(T string) {
        { string.cbegin() }
        noexcept->std::same_as<typename T::iterator>;
        { string.cend() }
        noexcept->std::same_as<typename T::iterator>;
    };

    template <class T>
    concept string_observer = std::movable<T> && (old_const_iterators<T>
                                                  /* || const_range_iterators<T>, in development */)
        //        && c_str<T> // Needed ???
        ;


    template <class T>
    concept pair = std::movable<T> /* && has two template args, in development */;


    //    template <class T>
    //    concept option = requires {
    //        requires std::same_as<decltype(T::name), custom_string_type>;
    //        requires std::same_as<decltype(T::description), custom_string_type>;
    //        requires std::same_as<decltype(T::short_name),
    //        custom_optional_type<custom_string_type>>;
    //    };

}  // namespace cxx20opts::concepts

#endif


namespace cxx20opts {


    namespace detail {
        constexpr static inline std::string_view separator = "-";
        constexpr static inline std::string_view double_separator = "--";
        constexpr static inline std::string_view windows_separator = "/";
    }  // namespace detail


    struct option {  // def impl, in development
        std::optional<std::string_view> short_name{std::nullopt};
        std::optional<std::string_view> name{std::nullopt};
        std::optional<std::string_view> description{std::nullopt};
    };


#ifdef cxx20opts_has_concepts
    //         option(concepts::string_observer auto /*str*/) {}
    // move this to ctor args / to template parametr, with default - str view?
    //        concepts::string_observer auto name;
    //        concepts::string_observer auto description;
    //        concepts::custom_optional_type<concepts::custom_string_type> auto description;

    //    static_assert(concepts::option<option>);
#endif

    struct raw_args {
        using count_t = int;
        using c_array_string_t = char**;

        count_t argc;
        c_array_string_t argv;
    };

    struct enable_windows_like_argument_t {};
    struct disable_windows_like_argument_t {};

    constexpr static inline enable_windows_like_argument_t enable_windows_like_argument{};
    constexpr static inline disable_windows_like_argument_t disable_windows_like_argument{};


    class options {
    public:
        using raw_args_t = raw_args;

        using count_t = raw_args::count_t;
        using c_array_string_t = raw_args::c_array_string_t;

        options() noexcept = default;
        options(const count_t argc_, char** argv_) noexcept : raw_{argc_, argv_} { parse_impl(); }

        auto parse(const count_t argc_, char** argv_) noexcept -> options&;
        auto enable_windows_like_argument() noexcept -> options&;
        auto disable_windows_like_argument() noexcept -> options&;
        auto operator()(const option&) -> options&;
        auto operator()(option&&) noexcept -> options&;
        auto add(const option&) -> options&;
        auto add(option&&) noexcept -> options&;

        auto operator[](const count_t) const noexcept -> std::string_view;
        auto operator[](std::string_view) const noexcept -> std::string_view;

        auto at(const count_t) const -> std::string_view;    /* throws std::out_of_range */
        auto at(std::string_view) const -> std::string_view; /* throws std::out_of_range */

        auto raw() const noexcept -> const raw_args_t&;

    private:
        bool enabled_windows_like_argument_opts{false};
        raw_args_t raw_;
        std::vector<std::optional<option>> opts;


        auto parse_impl() noexcept -> void;
    };

#ifdef cxx20opts_has_concepts
    //    auto operator[](concepts::string_observer auto str) const noexcept -> std::string_view;
#endif


    inline auto options::parse(const options::count_t argc_, char** argv_) noexcept -> options& {
        raw_ = raw_args_t{argc_, argv_};
        parse_impl();
        return *this;
    }


    inline auto options::enable_windows_like_argument() noexcept -> options& {
        enabled_windows_like_argument_opts = true;
        return *this;
    }
    inline auto options::disable_windows_like_argument() noexcept -> options& {
        enabled_windows_like_argument_opts = false;
        return *this;
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

    inline auto options::add(option&& o) noexcept -> options& {
        opts.emplace_back(std::move(o));
        return *this;
    }

    // a copy will be created option  /* throws std::bad_alloc */
    inline auto options::add(const option& o) -> options& {
        opts.push_back(o);
        return *this;
    }


    // unchecked method
    inline auto options::operator[](const options::count_t n) const noexcept -> std::string_view {
        return {raw_.argv[n]};
    }

    // unchecked method
    inline auto options::operator[](std::string_view str) const noexcept -> std::string_view {
        // TODO: unimplemented

        (void)(str);
        return {"unimplemented"};
    }

    /* throws std::out_of_range */
    inline auto options::at(const options::count_t n) const -> std::string_view {
        if (n > raw_.argc) {
            throw std::out_of_range("");
        }
        return {raw_.argv[n]};
    }

    /* throws std::out_of_range */
    inline auto options::at(std::string_view str) const -> std::string_view {
        // TODO: unimplemented

        (void)(str);
        return {"unimplemented"};
    }

    inline auto options::raw() const noexcept -> const raw_args_t& { return raw_; }



    inline auto options::parse_impl() noexcept -> void {
        // TODO: unimplemented
    }



    inline auto operator|(options& z, option&& o) noexcept -> options& {
        z.add(std::move(o));
        return z;
    }
    inline auto operator|(options& z, const option& o) -> options& {
        z.add(o);
        return z;
    }

    inline auto operator|(options& z, enable_windows_like_argument_t&&) noexcept -> options& {
        z.enable_windows_like_argument();
        return z;
    }
    inline auto operator|(options& z, const enable_windows_like_argument_t&) noexcept -> options& {
        z.enable_windows_like_argument();
        return z;
    }
    inline auto operator|(options& z, disable_windows_like_argument_t&&) noexcept -> options& {
        z.disable_windows_like_argument();
        return z;
    }
    inline auto operator|(options& z, const disable_windows_like_argument_t&) noexcept -> options& {
        z.disable_windows_like_argument();
        return z;
    }


}  // namespace cxx20opts

#endif

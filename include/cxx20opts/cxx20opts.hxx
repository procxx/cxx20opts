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
#include <optional>


#ifdef cxx20opts_has_concepts

//#if __has_include(<concepts>)
//#include <concepts>
//#endif

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

    namespace intrusive {

        /*
         * @bref lightweight intrusive wrapper
         */
        template <class T, class U>
        class pair {
        public:
            constexpr pair(T& t_, U& u_) noexcept : t{t_}, u{u_} {}
            constexpr pair(pair& p) noexcept : t{p.t}, u{p.u} {}
            constexpr pair(pair&& p) noexcept : t{p.t}, u{p.u} {}
            constexpr pair operator=(pair& p) noexcept {
                if (&p not_eq this) {
                    p.t = t;
                    p.u = u;
                }
                return *this;
            }
            constexpr pair operator=(pair&& p) noexcept {
                if (&p not_eq this) {
                    p.t = t;
                    p.u = u;
                }
                return *this;
            }

            constexpr auto first() noexcept -> T& { return t; }
            constexpr auto second() noexcept -> U& { return u; }
            constexpr auto first() const noexcept -> T& { return t; }
            constexpr auto second() const noexcept -> U& { return u; }

        private:
            T& t;
            U& u;
        };

    }  // namespace intrusive


    struct option {  // def impl, in development
    public:
        std::optional<std::string> short_name{std::nullopt};
        std::string name{};
        std::string description{};
    };

    //         option(concepts::string_observer auto /*str*/) {}
    // move this to ctor args / to template parametr, with default - str view?
    //        concepts::string_observer auto name;
    //        concepts::string_observer auto description;
    //        concepts::custom_optional_type<concepts::custom_string_type> auto description;

    //    static_assert(concepts::option<option>);

    class options {
    public:
        using count_t = int;
        using raw_args_t = intrusive::pair<int, char**>;

        constexpr options(count_t argc_, char** argv_) noexcept
            : argc{argc_}, argv{argv_}, raw_{argc, argv} {}

        auto operator()(const option&) -> options&;
        auto operator()(option&&) noexcept -> options&;
        auto operator|(const option&) -> options&;
        auto operator|(option&&) noexcept -> options&;
        auto add(const option&) -> options&;
        auto add(option&&) noexcept -> options&;

        auto operator[](count_t) const noexcept -> std::string_view;
        auto operator[](std::string_view) const noexcept -> std::string_view;

        auto at(count_t) const -> std::string_view;          /* throws std::out_of_range */
        auto at(std::string_view) const -> std::string_view; /* throws std::out_of_range */

        constexpr auto raw() const noexcept -> const raw_args_t&;

    private:
        int argc;
        char** argv;
        raw_args_t raw_;
    };

    //        auto operator[](concepts::string_observer auto str) const noexcept ->
    //        std::string_view;

    inline auto options::operator()(option&& o) noexcept -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }
    inline auto options::operator()(const option& o) -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }

    inline auto options::operator|(option&& o) noexcept -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }
    inline auto options::operator|(const option& o) -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }

    inline auto options::add(option&& o) noexcept -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }
    inline auto options::add(const option& o) -> options& {
        // TODO: unimplemented
        (void)(o);
        return *this;
    }


    // unchecked method
    inline auto options::operator[](options::count_t n) const noexcept -> std::string_view {
        return {argv[n]};
    }

    inline auto options::operator[](std::string_view str) const noexcept -> std::string_view {
        // TODO: unimplemented
        (void)(str);
        return {"unimplemented"};
    }


    /* throws std::out_of_range */
    inline auto options::at(options::count_t n) const -> std::string_view {
        if (n > argc) {
            throw std::out_of_range("");
        }
        return {argv[n]};
    }

    /* throws std::out_of_range */
    inline auto options::at(std::string_view) const -> std::string_view {
        // TODO: unimplemented
        return {"unimplemented"};
    }

    constexpr inline auto options::raw() const noexcept -> const raw_args_t& { return raw_; }


}  // namespace cxx20opts

#endif

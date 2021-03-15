#include <cxx20opts/cxx20opts.hxx>
#include <concepts>
#include <memory>
#include <limits>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <iostream>


using namespace cxx20opts;

// for test concepts
template <class CharT = char>
class basic_str_view {  // minimal string view api, psevdo-impl
public:
    using value_type = CharT;
    using size_t = decltype(sizeof 0);
    using difference_type = int;
    using const_reference = const value_type&;
    using const_pointer = const value_type*;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;  // std::random_access_iterator_tag?
        using size_t = decltype(sizeof 0);
        using difference_type = int;
        using const_reference = const value_type&;
        using const_pointer = const value_type*;

        explicit iterator(size_t _num = 0);

        iterator& operator++();
        iterator operator++(difference_type);

        auto operator==(iterator other) const noexcept -> bool;
        auto operator!=(iterator other) const noexcept -> bool;

        auto operator*() const -> const_reference;

    private:
        size_t num = 0;
    };

    auto cbegin() const noexcept -> iterator;
    auto cend() const noexcept -> iterator;

    constexpr basic_str_view() noexcept requires concepts::nothrow_default_ctorble<CharT> = default;
    constexpr ~basic_str_view() noexcept requires concepts::trivially_destructible<CharT> = default;
    constexpr auto c_str() const noexcept -> const_pointer;

private:
    const_pointer p;
};

using my_str_view = basic_str_view<char>;

// for test concepts
// Я уже не помню, зачем я написал этот клас...
// Вроде для теста подстановки кастомного класса пары. Но уже не помню куда его надо.
template <class First_t, class Second_t>
class mypair {
public:
    constexpr mypair() noexcept requires concepts::nothrow_default_ctorble<
        First_t>&& concepts::nothrow_default_ctorble<Second_t>
        : first{}, second{} {}

    constexpr mypair() requires concepts::default_constructible<
        First_t>&& concepts::default_constructible<Second_t>
        : first{}, second{} {}

    constexpr ~mypair() noexcept = default;

    constexpr mypair(First_t&& first_, Second_t&& second_) noexcept
        : first{std::move(first_)}, second{std::move(second_)} {}

    constexpr mypair(mypair&& pair) noexcept
        : first{std::move(pair.first)}, second{std::move(pair.second)} {}

    constexpr mypair& operator=(mypair&& pair) noexcept {
        if (pair == this) {
            return *this;
        }
        first = std::move(pair.first);
        second = std::move(pair.second);
        return *this;
    }

    constexpr mypair& operator=(const mypair&) = delete;
    constexpr mypair(const mypair&) = delete;


private:
    First_t first;
    Second_t second;
};


consteval auto tests_for_tests() -> int {
    static_assert(concepts::pair<mypair<int, char>>);
    static_assert(concepts::pair<mypair<char, char>>);
    static_assert(concepts::pair<mypair<my_str_view, my_str_view>>);
    static_assert(concepts::pair<mypair<char*, char**>>);

    static_assert(concepts::string_observer<my_str_view>);
    static_assert(concepts::string_observer<my_str_view>);
    static_assert(concepts::string_observer<my_str_view>);

    return 0;
}
constinit auto run_tests_for_tests = tests_for_tests();


int main(int argc, char* argv[] /* this is vla? */) {
    cxx20opts::options opts{argc, argv};

    opts.add(option{.short_name = "-h", .name = "help", .description = "Print help"});

    opts | option{.short_name = "f", .name = "file", .description = "Path to Some File"}  //
        | option{.name = "verbose", .description = "verbose output"}                      //
        | option{.short_name = "o", .name = "output", .description = "output file"}       //
        | option{.name = "version", .description = "show app version"};                   //

    opts | enable_windows_like_argument | disable_windows_like_argument |
        enable_windows_like_argument;

    option x{.name = " ", .description = " "};
    //    x.defautl_value<std::string_view>(""); // not implemented.

    opts.add(x);             // copy
    opts.add(std::move(x));  // move

    opts(option{.name = "kek", .description = "lol"})({})({})({});  // упрлся

    opts["kecasdak"];                          // unchecked UB, out of range, ubsan не ловит
    opts["v"], opts["version"];                // semantically equivalent
    opts.at("K"), opts.at("ADKFJDASKJFADHA");  // throws exception - out of range
    assert(opts.raw().argc == argc);           // get argc
    assert(opts.raw().argv == argv);           // get argv


    return 0;
}

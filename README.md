# cxx20opts
C++20 (17) command line parsing library

# Tasks:
  * Basic argument processing - wrapping with `std::string_view` (since C++17) (don't usage C++ `std::string` - redundant memory allocation).
  * Help generator, with custom options, user-defind, default - basic help output.
  * Option With small `-f path` and full name `--file path`. Space or ( and? ) equals sign (user defind, default - space) - separator option and argument.
  * Exception free overloading.
  * Optional windwos style command line argumets generate.
  * 

# Basic API (is disscussed):
  * class options - observer argc, argv.
```cpp
cxx20opts::options opts{argc, argv};
```
  * `obj[ "keyname"sv ]`, `obj[ arg_number - decltype (sizeof 0) ]`, `.at<T>(sv/arg_number)` - as default, returned `std::string_view` or `concept` (since C++20) string (is abstract string, user impl-defined). with support cast to user-defined type.
```cpp
opts["file"sv] // returned std::filesystem::path (if before defined)
```
  * `raw()` - get parir with raw `argc` && `argv`.
```cpp
obj.raw() -> std::pair<int, char**> /* or concept pair, default usage - std::pair */ 
```
  * `add_option()`
```cpp
options.add_option(option{}).add_option({});

// or options.option(option{})

// or options.option(option{}
| option{}
| option{}
| option{}
);
```
* `options.add_options(); // Is this really necessary?`

* etc...



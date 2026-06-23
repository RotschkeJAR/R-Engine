# Naming Conventions

The naming conventions defined here only apply to everything, that is defined within this software and exposed to other.
Datatypes or functions defined by external software should not be aliased by something, that renames or modifies its name.

Letters followed by an underscore are written in lower case, unless specified otherwise.

## Variables

All variables are written in Camel Case.

### Prefixes

All prefixes are applied in the order they're listed here except for pointers and arrays.
A prefix applies on the next, from left to right, e.g. the name `pacName` is a pointer to an array of characters (here: C-style string `char*` or `char (*)[]`), but `apcNames` is an array of pointers to a character (`char*[]`).

#### API Definitions

If any datatype, including structures, classes and typedefs, is defined by an API, the variable name has to carry the abbreviated name of the library followed by an underscore.

Only one API name can be included into the name at a time.

| Prefix | API/Library             | Platfrom    |
|--------|-------------------------|-------------|
| win64  | Windows                 | Windows     |
| x11    | X.org/X11/X server      | Linux       |
| wl     | Wayland                 | Linux       |
| xdg    | XDG                     | Linux       |
| vk     | Vulkan                  | *           |

#### Datatype

Variables, that represent primitive or any other important datatype have to contain a prefix to hint at their underlying datatype.
Only one datatype prefix can exist at a time.
The symbol `*` highlights, that a number can follow the prefix telling a specialization about the datatype.
Dangling zeros have to be discarded, e.g. `08` is illegal.
These prefixes should never start in upper case, if another prefix was written before, e.g. `Pp` for a double pointer (`void**`) is illegal.

##### Pointer

The letter `p` has to be added before the prefix of the underlying datatype, e.g. `pu8` equals `uint8_t*`.
The prefix can be stacked as many times as the C/C++ standard allows, that `ppd` (= `double**`) can also be valid.

Void-pointers (`void*`) have to be named `h` as they're treated as handles.

Function pointers have to contain `pv`. This rule doesn't apply to function pointers used for libraries, e.g. functions retrieved from Vulkan do not need it.

##### Array

For arrays the letter `a*` is used and optionally the number of elements the array can store.
The prefix can be stacked as many times as the C/C++ standard allows, that `aau` (= `unsigned int [][]`) can also be valid.

It's recommended to not add the number of elements to allow extending it when needed.
This rule also applies to pointers pointing at empty structures or classes, e.g. `struct Handle {}`.

##### Signed Integrals

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `si`   | `short int`                                   |                                                                                                           |
| `i`    | `int`                                         |                                                                                                           |
| `li`   | `long int`                                    |                                                                                                           |
| `lli`  | `long long int`                               |                                                                                                           |
| `ss`   | `ssize_t`                                     |                                                                                                           |
| `i*`   | `int8_t`, `int16_t`, `int32_t`, `int64_t`     | The number has to be equal to its respective integer type, e.g. `i8` always equals `int8_t`.              |

##### Unsigned Integrals

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `su`   | `unsigned short int`                          |                                                                                                           |
| `u`    | `unsigned int`                                |                                                                                                           |
| `lu`   | `unsigned long int`                           |                                                                                                           |
| `llu`  | `unsigned long long int`                      |                                                                                                           |
| `s`    | `size_t`                                      |                                                                                                           |
| `u*`   | `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` | The number has to be equal to its respective integer type, e.g. `u8` always equals `uint8_t`.             |

##### Masking

For bitmasks the unsigned integrals have to be used and contain the prefix `m*`.
If known the number tells how many bits the variable can store.

The datatypes defined by C, such as `unsigned int`, can be used, unless their minimum storage size in bits is smaller than the required amount.

##### Floating-point numbers

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `f`    | `float`                                       |                                                                                                           |
| `d`    | `double`                                      |                                                                                                           |
| `ld`   | `long double`                                 |                                                                                                           |

##### Boolean

For regular booleans the letter `b*` has to be appended to the variable name.

If a datatype other than `bool` is used, but treated as a boolean the letter `b` has to be appended and optionally the number of occupied bits, e.g. `b32` is either `int32_t` or `uint32_t`. This rule only applies to all datatypes, that can be used in boolean algebra.

##### Characters & Strings

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `c`    | `char`                                        |                                                                                                           |
| `wc`   | `wchar_t`                                     |                                                                                                           |
| `str`  | `std::string`                                 |                                                                                                           |
| `wstr` | `std::wstring`                                |                                                                                                           |

### Suffixes

\[This section doesn't exist\]

## Functions

All functions are written in Snake Case.

## Macros

Macros are written in Screaming Snake Case.
# Coding Conventions

This document covers the rules on how source code has to be written for this software to keep read- and maintainability.
The coding conventions listed here do apply to the internals and the interface defined by this software.

Elements, which were defined by APIs or any other software, are not allowed to be aliased to cover the convention rules.
The following datatypes are allowed to be aliased into the given names:
* `signed char`: `schar`
* `long long`: `llong`
* `unsigned char`: `uchar`
* `unsigned short`: `ushort`
* `unsigned int`: `uint`
* `unsigned long`: `ulong`
* `unsigned long long`: `ullong`

## Naming

The naming conventions defined here only apply to everything, that is defined within this software and exposed to other.
Datatypes or functions defined by external software should not be aliased.

Letters followed by an underscore are written in lower case, unless specified otherwise.

### Classes & Structs

The names of all classes and structs are written in Camel Case, but the first letter is in upper case.

### Concepts

All concepts are written in Camel Case, but the first letter is in upper case.

### Templates

Template functions, classes and parameters are written in the same way as defined per.
An extensible paramter pack doesn't have any special rules.

### Typedefs

A new name for an existing datatype has to contain the suffix `_t`, unless the datatype is a pointer. In that case the suffix `_p` has to be used.
The name has to be written in Camel Case.

#### Handles

Typedefs for defining handles are an exception to this and are written in Camel Case, but the first letter is in upper case.
This includes defining a pointer to an empty struct.

#### Flags

Another exception to the usual rule for typedefs are flags.
They are written in Camel Case, but the first letter is in upper case.

### Variables

All variables are written in Camel Case.

#### Prefixes

All prefixes are applied in the order they're listed here except for pointers and arrays.
A prefix applies on the next, from left to right, e.g. the name `pacName` is a pointer to an array of characters (here: C-style string `char*` or `char (*)[]`), but `apcNames` is an array of pointers to a character (`char*[]`).

##### API Definitions

If any datatype, including structures, classes and typedefs, is defined by an API, the variable name has to carry the abbreviated name of the library followed by an underscore.

Only one API name can be included into the name at a time.

| Prefix | API/Library             | Platfrom    |
|--------|-------------------------|-------------|
| `win`  | Windows                 | Windows     |
| `x11`  | X.org/X11/X server      | Linux       |
| `wl`   | Wayland                 | Linux       |
| `xdg`  | XDG                     | Linux       |
| `vk`   | Vulkan                  | *           |
| `std`  | C/C++ standard library  | *           |

The `std` prefix should not be appended to variable names storing `size_t`. The datatype should also not contain the namespace `std`.
Therefore a codeline such as `std::size_t std_bytes` is illegal.

##### Datatype

Variables, that represent primitive or any other important datatype have to contain a prefix to hint at their underlying datatype.
Only one datatype prefix can exist at a time.
The symbol `*` highlights, that a number can follow the prefix telling a specialization about the datatype.
Dangling zeros have to be discarded, e.g. `08` is illegal.
These prefixes should never start in upper case, if another prefix was written before, e.g. `Pp` for a double pointer (`void**`) is illegal.

If the datatype is selected by the compiler due to the keyword `auto`, no rule applies and only the prefix `x` is used.
No assumptions should be made.

If a datatype is defined by `typedef`, no prefix telling the underlying datatype will be appended, unless its purpose is clear.

###### Reference

The variable name of a reference has to contain the letter `r`.
Due to the nature it cannot be stacked like pointers.

An rvalue has to carry the prefix `rr`.

Wrappers provided by C++ are not affected by this rule.

###### Pointer

The letter `p` has to be added before the prefix of the underlying datatype, e.g. `pu8` equals `uint8_t*`.
The prefix can be stacked as many times as the C/C++ standard allows, that `ppd` (= `double**`) can also be valid.

Void-pointers (`void*`) have to be named `h` as they're treated as handles.
This rule also applies to pointers pointing at empty structures or classes, e.g. `struct Handle {}`.

Function pointers have to contain `pv`. This rule doesn't apply to function pointers used for libraries, e.g. functions retrieved from Vulkan do not need it.

Smart pointers such as `unique_ptr` are not affected by this rule.

###### Array

For arrays the letter `a*` is used and optionally the number of elements the array can store.
The prefix can be stacked as many times as the C/C++ standard allows, that `aau` (= `unsigned int [][]`) can also be valid.

It's recommended to not add the number of elements to allow extending it when needed.

The `array` class in C++ is not affected by this rule.

###### Signed Integrals

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `sc`   | `signed char`                                 |                                                                                                           |
| `si`   | `short int`                                   |                                                                                                           |
| `i`    | `int`                                         |                                                                                                           |
| `li`   | `long int`                                    |                                                                                                           |
| `lli`  | `long long int`                               |                                                                                                           |
| `ss`   | `ssize_t`                                     |                                                                                                           |
| `i*`   | `int8_t`, `int16_t`, `int32_t`, `int64_t`     | The number has to be equal to its respective integer type, e.g. `i8` always equals `int8_t`.              |

###### Unsigned Integrals

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `uc`   | `unsigned char`                               |                                                                                                           |
| `su`   | `unsigned short int`                          |                                                                                                           |
| `u`    | `unsigned int`                                |                                                                                                           |
| `lu`   | `unsigned long int`                           |                                                                                                           |
| `llu`  | `unsigned long long int`                      |                                                                                                           |
| `s`    | `size_t`                                      |                                                                                                           |
| `u*`   | `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` | The number has to be equal to its respective integer type, e.g. `u8` always equals `uint8_t`.             |

###### Masking

For bitmasks the unsigned integrals have to be used and contain the prefix `m*`.
If known the number tells how many bits the variable can store.

The datatypes defined by C, such as `unsigned int`, can be used, unless their minimum storage size in bits is smaller than the required amount.

This rule does not apply to `bitset` from C++.

###### Floating-point numbers

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `f`    | `float`                                       |                                                                                                           |
| `d`    | `double`                                      |                                                                                                           |
| `ld`   | `long double`                                 |                                                                                                           |

###### Boolean

For regular booleans the letter `b*` has to be appended to the variable name.

If a datatype other than `bool` is used, but treated as a boolean the letter `b` has to be appended and optionally the number of occupied bits, e.g. `b32` is either `int32_t` or `uint32_t`. This rule only applies to all datatypes, that can be used in boolean algebra.

###### Characters & Strings

| Prefix | Datatype                                      | Notes                                                                                                     |
|--------|-----------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `c`    | `char`                                        |                                                                                                           |
| `c*`   | `char8_t`, `char16_t`, `char32_t`             | The number has to be equal to its respective character type, e.g. `c32` always equals `char32_t`.         |
| `wc`   | `wchar_t`                                     |                                                                                                           |
| `t`    | `std::string`                                 |                                                                                                           |
| `wt`   | `std::wstring`                                |                                                                                                           |

###### Enumerations

The prefix `e` has to be inserted, if the underlying datatype is an enumeration.
The used enumeration should not be named.

If a variable stores a bitmask, that is a combination of enumeration values, the rule for Masking applies.

#### Suffixes

\[This section doesn't exist\]

### Functions

All functions are written in Snake Case.

Invoked functions provided by the C/C++ standard library are required to have the `std` namespace explicitly written, e.g. `std::printf` instead of `printf`.

### Macros

Macros are written in Screaming Snake Case.

## Indentation

A tab has to be 4 spaces wide and any coding tool used to write should differentiate between spaces and tabs and enter the appropriate character, that has been selected via keyboard.
An IDE, that inserts spaces when tab is pressed, should be reconfigured to match the rules.

### Preprocessor Macros

For macros only spaces are allowed to be used.
When the definition of a macro is written, it has to be indented and aligned with the other neighbouring lines.
If it continues on subsequent lines after a backslash, it has to be indented by 8 spaces or 2 tabs extra.

```C++
#define FIRST     _1st
#define SECOND    _2ns
#define THIRD     _3rd
#define FOURTH    _4th
#define FIFTH     _5th

#define MULTILINE    (multiple \
		+ line \
		+ in \
		+ here)
```

Preprocessor commands such as `ifdef`, `define` and `endif` have to be indented as much as the context they belong to.

```C++
namespace Namespace {

#define INSIDE_NAMESPACE 1

	void func() {
	#define INSIDE_NAMESPACE_FUNC 1
	}

}

void func() {
#define INSIDE_FUNC 1
}
```

The symbol `#` all start in the same column, but its commands have to be indented with spaces, if they're nested.

```C++
#ifdef NDEFINED
# undef NDEFINED
#endif
```

### Variables

New variable definitions, that are in subsequent lines, have to be indented by 1 tab.
If the value, that is assigned to a variable, is split across multiple lines, they have to be indented by 2 tabs.

```C++
short siFirst,
	siSecond,
	siThird = (2 * 2)
			+ (2 / 2);
```

### Parameter List

The parameter list of any function, including constructors and macro functions, that continues in subsequent lines, have to be indented by 2 tabs.
Each line should contain only one parameter.

The initialization list of object variables in a constructor have to be indented 2 tabs.

```C++
void func(int iFirst,
		int iSecond) {
	/* ... */
}

class Class {
	public:
		const int iFirst,
			iSecond;

		Class(int iFirst,
				int iSecond)
				: iFirst(iFirst),
				iSecond(iSecond) {
			/* ... */
		}
};
```

### Structs

All members of a data structure have to be indented by 1 tab.

### Classes

The keywords `public`, `private` and `protected` have to be indented by 1 tab.
All members are indented by 2 tabs.

```C++
class Class {
	private:
		int iPrivateMember;

	protected:
		int iProtectedMember;

	public:
		int iPublicMember;
};
```

### Goto Markers

The goto-markers have to be indented 1 tab less than the content they're in.

```C++
void func() {
	goto GOTO_MARKER;
GOTO_MARKER:
	printf("Hello World");
	goto INSIDE_IF;
	if (false) {
	INSIDE_IF:
		printf("Inside bracket");
	}
}
```

## Header File Content

In the header files only the following are allowed to reside:

* External variable definitions
* Template functions
* `constexpr` variables
* `constexpr` or `consteval` functions
* Preprocessors
* Macro definitions
* Class definitions
* Enumeration definitions
* Function definitions

Inlining is not allowed, unless it fulfills one of the above requirements and is necessary.
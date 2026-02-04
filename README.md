# Vix Conversion

The **conversion module** provides fast, explicit, and allocation-aware helpers
to convert **strings to typed values** and **typed values to strings** in C++.

It is designed as a low-level building block for:
- parsers
- configuration systems
- CLI tools
- HTTP APIs
- validation pipelines (used by `vix::validation`)

The module favors **predictability**, **performance**, and **explicit error handling**.

---

## Philosophy

Conversion in Vix follows strict rules:

- No exceptions
- No implicit conversions
- No locale dependence
- No iostreams
- Precise error reporting (code, position, input)

Every conversion returns an `expected<T, ConversionError>`.

---

## Core Concepts

| Concept | Purpose |
|------|-------|
| `to_int<T>` | Parse integer from string |
| `to_float<T>` | Parse floating point from string |
| `to_bool` | Parse boolean values |
| `to_enum<T>` | Parse enum using explicit mapping |
| `parse<T>` | Generic typed parsing facade |
| `to_string` | Convert values to string |
| `ConversionError` | Structured conversion error |
| `expected<T,E>` | Explicit success / failure |

---

## Error Model

All conversions return:

```cpp
expected<T, ConversionError>
```

Where `ConversionError` contains:

- `code` — semantic error code
- `position` — error index in input
- `input` — original input

Example:

```cpp
static void print_err(const ConversionError &e)
{
  std::cout << "error: " << to_string(e.code)
            << " position=" << e.position
            << " input='" << e.input << "'\n";
}
```

---

## Parsing Integers

```cpp
#include <vix/conversion/ToInt.hpp>

auto r = to_int<int>("  -42  ");
if (!r)
{
  // handle r.error()
}
else
{
  int value = r.value();
}
```

Examples:
- `examples/parse_int.cpp`
- `examples/parse_generic.cpp`

---

## Parsing Floating Point Values

```cpp
#include <vix/conversion/ToFloat.hpp>

auto r = to_float<double>(" 3.14 ");
```

Supports:
- scientific notation
- leading / trailing spaces

Examples:
- `examples/parse_float.cpp`

---

## Parsing Booleans

Accepted values (case-insensitive):
- `true`, `false`
- `1`, `0`
- `yes`, `no`
- `on`, `off`

```cpp
#include <vix/conversion/ToBool.hpp>

auto r = to_bool("yes");
```

Examples:
- `examples/parse_bool.cpp`

---

## Parsing Enums

Enums are parsed using an explicit mapping table.

```cpp
enum class Role { Admin, User };

static constexpr EnumEntry<Role> roles[] = {
  {"admin", Role::Admin},
  {"user",  Role::User},
};

auto r = to_enum<Role>("ADMIN", roles); // case-insensitive
```

Examples:
- `examples/parse_enum.cpp`

---

## Generic Parsing

`parse<T>` automatically selects the correct converter.

```cpp
#include <vix/conversion/Parse.hpp>

auto a = parse<int>("42");
auto b = parse<double>("3.14");
auto c = parse<bool>("true");
```

Examples:
- `examples/parse_generic.cpp`

---

## Converting Values to String

```cpp
#include <vix/conversion/ToString.hpp>

auto a = to_string(42);
auto b = to_string(3.14);
auto c = to_string(true);
```

Enums require a mapping table:

```cpp
auto r = to_string(Role::Admin, roles);
```

Examples:
- `examples/to_string.cpp`

---

## Performance Notes

- Uses `std::from_chars` / `std::to_chars`
- No heap allocations in parsing paths
- Stack buffers only
- Locale-independent

This makes the module suitable for hot paths and low-latency systems.

---

## Tests

Unit tests are located in `tests/`:

- `to_int_smoke.cpp`
- `to_float_smoke.cpp`
- `to_bool_smoke.cpp`
- `to_enum_smoke.cpp`

Run:

```bash
ctest
```

---

## Relationship with Validation

The `conversion` module is the foundation of `vix::validation`:

- `ParsedValidator<T>` relies on `parse<T>`
- Form binding depends on conversion error reporting
- Errors propagate without loss of information

---

## Design Goals Recap

- Explicit over implicit
- Errors as values
- No runtime surprises
- Easy to embed
- Predictable behavior

---

## License

MIT
Part of the **Vix.cpp** project.


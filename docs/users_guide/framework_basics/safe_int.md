# `safe_int<T>` — Overflow-Safe Integer Arithmetic

## Overview

`safe_int<T>` is a thin wrapper around an integral type `T` that **detects all arithmetic
overflow at runtime**. It participates in the ordinary C++ arithmetic system and satisfies
all the concepts that **mp-units** requires of a
[representation type](representation_types.md) — making it a true drop-in replacement for
plain integers.

```cpp
#include <mp-units/safe_int.h>
using namespace mp_units;

// Just change the representation type — everything else stays the same
quantity<mm, safe_i32> distance{1'500 * km};
quantity<mm, safe_i32> doubled = distance + distance;  // throws — 3×10⁹ > INT32_MAX
```

!!! info "Motivation"

    **mp-units**' [built-in scaling algorithm](representation_types.md#built-in-scaling-algorithm)
    uses widened intermediate arithmetic (`int64_t` for types up to `int32_t`, 128-bit for
    `int64_t`) to avoid undefined behavior during unit conversions. This handles the vast
    majority of real-world scenarios, but the **final result** must still fit in the target
    type — and that narrowing can overflow silently.

    `safe_int<T>` closes this gap: it checks **every** arithmetic operation, including
    the final narrowing, so overflow is never silent.


## Checked operations

Every arithmetic operation on `safe_int<T>` is checked before it executes:

| Operation | Overflow condition checked                                               |
|-----------|--------------------------------------------------------------------------|
| `a + b`   | signed: both-same-sign sum crosses boundary; unsigned: `lhs > max - rhs` |
| `a - b`   | symmetric to addition                                                    |
| `a * b`   | widened multiplication; result outside `[min, max]`                      |
| `a / b`   | divide-by-zero; signed: `INT_MIN / -1`                                   |
| `-a`      | signed: `INT_MIN`; unsigned: any non-zero                                |

The multiplication check uses widened intermediate arithmetic (e.g., `int32_t` promotes to
`int64_t` for the product), so there is no dependency on undefined behavior.


## Policy-based error handling

What happens when overflow is detected is controlled by the `ErrorPolicy` template
parameter:

```cpp
template<std::integral T, typename ErrorPolicy = /* see below */>
class safe_int;
```

**mp-units** ships two policies:

| Policy                      | Behaviour                    | Environment           |
|-----------------------------|------------------------------|-----------------------|
| `safe_int_terminate_policy` | `std::abort()` immediately   | freestanding + hosted |
| `safe_int_throw_policy`     | throws `std::overflow_error` | hosted only           |

The default policy is `safe_int_throw_policy` on hosted platforms and
`safe_int_terminate_policy` on freestanding platforms.

### Convenience aliases

All standard fixed-width integer aliases are provided with the default policy:

```cpp
using safe_i8  = mp_units::safe_int<std::int8_t>;
using safe_i16 = mp_units::safe_int<std::int16_t>;
using safe_i32 = mp_units::safe_int<std::int32_t>;
using safe_i64 = mp_units::safe_int<std::int64_t>;
using safe_u8  = mp_units::safe_int<std::uint8_t>;
using safe_u16 = mp_units::safe_int<std::uint16_t>;
using safe_u32 = mp_units::safe_int<std::uint32_t>;
using safe_u64 = mp_units::safe_int<std::uint64_t>;
```

For explicit policy control, use the full template:

```cpp
safe_int<std::int32_t, safe_int_throw_policy> explicit_throw;
safe_int<std::int32_t, safe_int_terminate_policy> explicit_terminate;
```

### Custom error policies

You can define your own error policy to integrate with custom logging or diagnostics
systems:

```cpp
#include <mp-units/safe_int.h>

struct logging_policy {
  [[noreturn]] static void on_overflow(std::string_view msg)
  {
    log_critical_error("Arithmetic overflow", msg);
    std::abort();
  }
};

using logged_int = mp_units::safe_int<std::int32_t, logging_policy>;
```

The policy must provide a `static void on_overflow(std::string_view)` method.

!!! tip "Shared policies with `constrained<T>`"

    If you also use
    [`quantity_bounds`](the_affine_space.md#range-validated-quantity-points) for range
    validation, you can add `on_constraint_violation(std::string_view)` to the same
    policy type — both `safe_int` and `constrained` will use it. See
    [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md) for
    a complete example.


## Integral promotion rules

`safe_int<T>` preserves C++ integral promotion behavior — adding two `safe_int<int16_t>`
values produces `safe_int<int>`, exactly matching what happens with the underlying types:

```cpp
// Underlying types: int16_t + int16_t → int (integral promotion)
static_assert(std::is_same_v<decltype(std::int16_t{1} + std::int16_t{1}), int>);

// safe_int preserves this: safe_int<int16_t> + safe_int<int16_t> → safe_int<int>
static_assert(std::is_same_v<decltype(safe_i16{1} + safe_i16{1}), safe_int<int>>);

// This propagates through quantity arithmetic:
static_assert(std::is_same_v<decltype(safe_i16{1} * si::metre + safe_i16{1} * si::metre),
                             quantity<si::metre, safe_int<int>>>);
```

This ensures that `safe_int` acts as a **transparent wrapper** — it adds overflow detection
without changing the fundamental arithmetic behavior.


## Drop-in replacement

`safe_int<T>` satisfies all the same [representation concepts](concepts.md#RepresentationOf)
as `T`. Only the representation type changes — everything else stays identical:

=== "plain int16_t"

    ```cpp
    quantity<si::metre, std::int16_t> q{30'000 * si::metre};
    quantity<si::metre, std::int16_t> doubled{q + q};  // ⚠️ overflows silently
    ```

=== "safe_int<T>"

    ```cpp
    quantity<si::metre, safe_i16> q{30'000 * si::metre};
    quantity<si::metre, safe_i16> doubled{q + q};  // throws std::overflow_error ✓
    ```

The overflow is caught because `q + q` promotes to `safe_int<int>` via integral promotion
(just as `int16_t + int16_t → int`), and the `quantity<si::metre, safe_i16>` constructor
narrows the result back to `int16_t` — that narrowing is where `safe_int` detects that
60,000 doesn't fit and throws.


## Where overflow is caught?

Because `safe_int` hooks into the fundamental C++ arithmetic operators, every operation
is checked — regardless of context:

=== "Construction"

    ```cpp
    int value = 40'000;

    // Constructing a safe_int quantity from a plain integer that doesn't fit the rep type
    quantity<si::metre, safe_i16> q{value * si::metre};  // throws — 40,000 > INT16_MAX
    ```

=== "Conversion"

    ```cpp
    quantity q = 40'000 * si::metre;

    // Converting from a quantity with too large numerical value for the rep type
    quantity<si::metre, safe_i16> q_safe{q};  // throws — 40,000 > INT16_MAX
    ```

=== "Arithmetic"

    ```cpp
    // Same-unit addition — no unit conversion, plain safe_int arithmetic
    quantity dist = safe_i32{2'000'000'000} * si::metre;
    quantity total = dist + dist;         // throws — 4×10⁹ overflows int32_t

    // Cross-quantity multiplication: speed × time
    quantity speed = safe_i32{50'000} * (si::metre / si::second);
    quantity time  = safe_i32{50'000} * si::second;
    quantity distance = speed * time;     // throws — 2.5×10⁹ overflows int32_t
    ```

=== "Explicit unit conversion"

    ```cpp
    quantity dist = safe_i32{2'200'000} * si::metre;
    quantity huge = dist.in(si::micro<si::metre>);  // throws — ×1,000,000 factor overflows int32_t
    ```

=== "Automatic common-unit scaling"

    ```cpp
    quantity dist_m  = safe_i32{1'500'000'000} * si::metre;
    quantity dist_km = safe_i32{1'000'000} * si::kilo<si::metre>;
    quantity total = dist_m + dist_km;    // throws — scaling 10⁶ km → m overflows int32_t
    ```


## Comparisons

Comparison operators (`==`, `<`, `>`, etc.) are structurally safer than arithmetic: they
return only `bool`, so the widened intermediate values are used to produce the boolean
result and then discarded. **mp-units** widens the intermediate freely (to `int64_t` or
128-bit), so for the vast majority of practical scenarios comparisons are correct with
plain integer types — `safe_int` adds no extra benefit here.

For extreme corner cases where even 128-bit intermediates overflow, see the `static_assert`
examples in the
[Preventing Integer Overflow](../../blog/posts/preventing-integer-overflow.md#approach-3-mp-units-widened-intermediate-arithmetic)
blog post.


## `constexpr` support

`safe_int<T>` arithmetic is fully `constexpr`. In C++, any overflow that occurs during
constant expression evaluation is always a compile-time hard error — for both `safe_int`
and plain integers. The difference emerges only at **runtime**, where `safe_int` catches
overflows that plain integers silently ignore.


## Relation to `constrained<T, Policy>`

`safe_int<T, EP>` follows the same policy-based design as
[`constrained<T, EP>`](the_affine_space.md#range-validated-quantity-points), the wrapper
used for `quantity_point` domain bounds. They address complementary concerns:

| Wrapper              | Purpose                                                           |
|----------------------|-------------------------------------------------------------------|
| `constrained<T, EP>` | **Value domain** — is this value within the declared bounds?      |
| `safe_int<T, EP>`    | **Arithmetic correctness** — does this operation fit in the type? |

Both share the same base error policies (`throw_policy`, `terminate_policy`) and extend
them with domain-specific handlers (`on_constraint_violation` for `constrained`,
`on_overflow` for `safe_int`). They compose naturally:

```cpp
// A bounded, overflow-safe latitude representation
using safe_double   = constrained<double, throw_policy>;
using safe_latitude = quantity_point<geo_latitude[deg], equator, safe_double>;

// A raw-integral distance type where overflow is caught at runtime
using safe_i32_m = quantity<isq::length[si::metre], safe_int<std::int32_t>>;
```

For a complete walkthrough of combining both wrappers, see
[Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md).

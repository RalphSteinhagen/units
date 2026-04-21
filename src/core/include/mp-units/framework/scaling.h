// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/fixed_point.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif


namespace mp_units {

namespace detail {

/**
 * @brief Intentionally silent narrowing cast with compiler float-conversion diagnostics suppressed.
 *
 * @tparam To    target type
 * @tparam From  source type (deduced)
 */
MP_UNITS_EXPORT template<typename To, typename From>
[[nodiscard]] constexpr To silent_cast(From value) noexcept
{
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_INT_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA
  return static_cast<To>(value);
  MP_UNITS_DIAGNOSTIC_POP
}

template<typename T>
inline constexpr bool treat_as_integral = !treat_as_floating_point<T>;

template<auto M, typename T>
[[nodiscard]] constexpr auto scale_fp(const T& v)
{
  using element_t = value_type_t<T>;
  static_assert(treat_as_floating_point<element_t>);
  if constexpr (is_integral(pow<-1>(M)) && !is_integral(M)) {
    // M has an integral inverse (pure divisor).  Prefer division over multiplication
    // to avoid the rounding errors introduced by 1/x in floating-point.
    constexpr element_t div = detail::silent_cast<element_t>(get_value<long double>(pow<-1>(M)));
    return v / div;
  } else {
    constexpr element_t ratio = detail::silent_cast<element_t>(get_value<long double>(M));
    return v * ratio;
  }
}

template<typename T>
constexpr decltype(auto) as_element(const T& value)
{
  if constexpr (std::convertible_to<T, value_type_t<T>>)
    return static_cast<value_type_t<T>>(value);
  else
    return value;
}

// The wider type used for magnitude constants to prevent overflow.
template<typename element_t>
using wider_int_for =
  std::conditional_t<(sizeof(element_t) <= sizeof(std::int32_t)), std::int64_t,
                     std::conditional_t<(sizeof(element_t) < sizeof(int128_t)), int128_t, element_t>>;

template<auto M, typename T>
[[nodiscard]] constexpr auto scale_int(const T& v)
{
  using element_t = value_type_t<T>;
  using wider_t = wider_int_for<element_t>;
  static_assert(treat_as_integral<element_t>);
  if constexpr (is_integral(M)) {
    constexpr wider_t mul = get_value<wider_t>(M);
    return v * mul;
  } else if constexpr (is_integral(pow<-1>(M))) {
    constexpr wider_t div = get_value<wider_t>(pow<-1>(M));
    return v / div;
  } else if constexpr (is_integral(M * (denominator(M) / numerator(M)))) {
    // M is a pure rational p/q (no irrational factors such as π).
    // Use wider_t for the numerator to prevent intermediate overflow in v * num.
    // The wider type propagates through the type's own operators:
    // plain int widens via C++ promotion rules, safe_int widens via its
    // checked operator* template, cartesian_vector widens element-wise, etc.
    constexpr wider_t num = get_value<wider_t>(numerator(M));
    constexpr element_t den = get_value<element_t>(denominator(M));
    return v * num / den;
  } else {
    // M has irrational factors (e.g. π): use long double fixed-point approximation.
    // fixed_point::scale operates on plain integers, so extract the scalar element.
    static_assert(std::convertible_to<T, element_t>,
                  "Scaling an integral-element wrapping type by an irrational magnitude factor "
                  "is not supported; use a floating-point element type instead");
    constexpr auto ratio = fixed_point<element_t>(get_value<long double>(M));
    return ratio.scale(as_element(v));
  }
}

}  // namespace detail

/**
 * @brief Scale @p value by the unit magnitude passed as @p m, converting to type @c To.
 *
 * When @p From provides a magnitude-aware @c operator*(From,M) customization point, it is
 * used first.  The return type may differ from @c To (e.g. a representation with scaled
 * bounds).  Otherwise, the built-in floating-point or integer path is
 * used and the result type is @c To.
 *
 * Use this in custom `operator*(T, UnitMagnitude)` implementations to reuse the
 * library's built-in scaling logic instead of duplicating it.
 */
MP_UNITS_EXPORT template<typename To, UnitMagnitude M, typename From>
  requires detail::MagnitudeScalable<From>
[[nodiscard]] constexpr auto scale(M m, const From& value)
{
  if constexpr (requires { value * m; }) {
    // Type provides magnitude-aware scaling via operator*(T, UnitMagnitude).
    return value * m;
  } else if constexpr (detail::UsesFloatingPointScaling<From> || detail::UsesFloatingPointScaling<To>) {
    // Floating-point path — handles both plain arithmetic types and wrappers.
    // Uses the type's own operator* / operator/ (element-wise for wrappers).
    // When From is integral (e.g. int → double), convert to To first so that
    // the FP scaling operates on the correct type.
    if constexpr (detail::treat_as_integral<detail::value_type_t<From>>)
      return detail::silent_cast<To>(detail::scale_fp<M{}>(static_cast<To>(value)));
    else
      return detail::silent_cast<To>(detail::scale_fp<M{}>(value));
  } else {
    // Integer arithmetic path (UsesIntegerScaling) — handles plain arithmetic types,
    // wrappers (safe_int), and containers (cartesian_vector<int>) alike.
    // For integral/pure-divisor magnitudes, uses the type's own operator* / operator/
    // (safe_int's overflow checks, cartesian_vector's element-wise operations).
    // For rational magnitudes, widens the factor to prevent intermediate overflow;
    // the wider factor propagates through the type's own operators.
    // static_cast<To> triggers the wrapper's checked narrowing constructor when needed.
    return static_cast<To>(detail::scale_int<M{}>(value));
  }
}

}  // namespace mp_units

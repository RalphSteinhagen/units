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
  MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
  MP_UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA
  return static_cast<To>(value);
  MP_UNITS_DIAGNOSTIC_POP
}

template<typename T>
inline constexpr bool treat_as_integral = !treat_as_floating_point<T>;

template<typename common_t, auto M, typename T>
[[nodiscard]] constexpr T scale_fp(T v)
{
  static_assert(treat_as_floating_point<common_t>);
  if constexpr (is_integral(pow<-1>(M)) && !is_integral(M)) {
    // M has an integral inverse (pure divisor).  Prefer division over multiplication
    // to avoid the rounding errors introduced by 1/x in floating-point.
    constexpr common_t div = detail::silent_cast<common_t>(get_value<long double>(pow<-1>(M)));
    return v / div;
  } else {
    constexpr common_t ratio = detail::silent_cast<common_t>(get_value<long double>(M));
    return v * ratio;
  }
}

template<typename common_t, auto M>
[[nodiscard]] constexpr common_t scale_int_scalar(common_t v)
{
  static_assert(treat_as_integral<common_t>);
  if constexpr (is_integral(M)) {
    constexpr common_t mul = get_value<common_t>(M);
    return v * mul;
  } else if constexpr (is_integral(pow<-1>(M))) {
    constexpr common_t div = get_value<common_t>(pow<-1>(M));
    return v / div;
  } else if constexpr (is_integral(M * (denominator(M) / numerator(M)))) {
    // M is a pure rational p/q (no irrational factors such as π).
    constexpr common_t num = get_value<common_t>(numerator(M));
    constexpr common_t den = get_value<common_t>(denominator(M));
    if constexpr (sizeof(common_t) <= sizeof(std::int32_t)) {
      // Use int64_t for all types up to int32_t: provides maximum safety headroom
      // with no performance cost on modern 64-bit systems.
      return static_cast<common_t>(static_cast<std::int64_t>(v) * num / den);
    } else if constexpr (sizeof(common_t) < sizeof(int128_t)) {
      // Use 128-bit arithmetic for int64_t.
      return static_cast<common_t>(static_cast<int128_t>(v) * num / den);
    } else {
      // At max integer width: no wider type available, compute in common_t directly.
      return v * num / den;
    }
  } else {
    // M has irrational factors (e.g. π): use long double fixed-point approximation.
    constexpr auto ratio = fixed_point<common_t>(get_value<long double>(M));
    return ratio.scale(v);
  }
}

template<typename common_t, auto M, typename T>
[[nodiscard]] constexpr T scale_int_wrapper(const T& v)
{
  static_assert(treat_as_integral<common_t>);
  if constexpr (is_integral(M)) {
    constexpr common_t mul = get_value<common_t>(M);
    return v * mul;
  } else if constexpr (is_integral(pow<-1>(M))) {
    constexpr common_t div = get_value<common_t>(pow<-1>(M));
    return v / div;
  } else if constexpr (is_integral(M * (denominator(M) / numerator(M)))) {
    constexpr common_t num = get_value<common_t>(numerator(M));
    constexpr common_t den = get_value<common_t>(denominator(M));
    return v * num / den;
  } else {
    // Wrapping type with irrational magnitude: no fallback without floating-point element type.
    static_assert(mp_units::treat_as_floating_point<common_t>,
                  "Scaling an integral-element wrapping type by an irrational magnitude factor "
                  "is not supported; use a floating-point element type instead");
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

}  // namespace detail

/**
 * @brief Scale @p value by the unit magnitude passed as @p m, converting to type @c To.
 *
 * When @p From provides a magnitude-aware @c operator*(From,M) customization point, it is
 * used first.  The return type may differ from @c To (e.g. a representation with scaled
 * bounds).  Otherwise, the built-in floating-point, fixed-point, or element-wise path is
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
    // At least one side is floating-point: compute with common_type_t precision.
    using common_t = std::common_type_t<value_type_t<From>, value_type_t<To>>;
    static_assert(treat_as_floating_point<common_t>);
    if constexpr (std::convertible_to<From, common_t>) {
      // Scalar (includes integer-to-FP): project to common_t so that scale_fp is
      // instantiated as scale_fp<common_t, M, common_t> and shared across all (From, To)
      // pairs with the same common_t.
      return detail::silent_cast<To>(detail::scale_fp<common_t, M{}>(static_cast<common_t>(value)));
    } else {
      // FP wrapping type (e.g. cartesian_vector<double>, std::complex<double>): pass as-is;
      // the wrapping type's own operator* / operator/ scales all elements.
      return detail::silent_cast<To>(detail::scale_fp<common_t, M{}>(value));
    }
  } else {
    // UsesFixedPointScaling or UsesElementWiseScaling: integer arithmetic.
    using common_t = std::common_type_t<value_type_t<From>, value_type_t<To>>;
    static_assert(detail::treat_as_integral<common_t>);
    if constexpr (detail::UsesFixedPointScaling<From>) {
      // Scalar: project to common_t and delegate to the shared integer-arithmetic helper.
      return static_cast<To>(detail::scale_int_scalar<common_t, M{}>(static_cast<common_t>(detail::as_element(value))));
    } else {
      // Wrapping type (UsesElementWiseScaling): delegate to the shared integer-wrapper helper.
      return static_cast<To>(detail::scale_int_wrapper<common_t, M{}>(value));
    }
  }
}

}  // namespace mp_units

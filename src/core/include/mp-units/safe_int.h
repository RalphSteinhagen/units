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

#include <mp-units/bits/fixed_point.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/constrained.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/fmt.h>
#endif
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/scaling.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>
#include <concepts>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <stdexcept>
#endif
#endif
#endif

namespace mp_units {

// ============================================================================
// OverflowPolicy concept
// ============================================================================

MP_UNITS_EXPORT template<typename EP>
concept OverflowPolicy = requires(std::string_view msg) { EP::on_overflow(msg); };

// ============================================================================
// Error policies (inherit base policies from constrained.h and add on_overflow)
// ============================================================================

/**
 * @brief Error policy that terminates the program on overflow (always available, freestanding-safe).
 */
MP_UNITS_EXPORT struct safe_int_terminate_policy : terminate_policy {
  [[noreturn]] static void on_overflow(std::string_view) noexcept { std::abort(); }
};

#if MP_UNITS_HOSTED

/**
 * @brief Error policy that throws std::overflow_error on overflow (hosted only).
 */
MP_UNITS_EXPORT struct safe_int_throw_policy : throw_policy {
  [[noreturn]] static void on_overflow(std::string_view msg) { throw std::overflow_error(std::string(msg)); }
};

#endif  // MP_UNITS_HOSTED

// ============================================================================
// Overflow detection helpers (only for std::integral T)
//
// NOTE: These helpers work on a SINGLE type T, not heterogeneous types.
// The calling code (in safe_int_binary_ops) handles integral promotion
// by first determining the promoted result type R = decltype(T() + U()),
// then calling add_overflows<R>(static_cast<R>(lhs), static_cast<R>(rhs)).
// This means both operands are converted to R before checking, which is
// consistent with how C++ arithmetic actually behaves.
// ============================================================================

namespace detail {

// Returns true if lhs + rhs overflows for signed/unsigned T.
template<std::integral T>
[[nodiscard]] constexpr bool add_overflows(T lhs, T rhs) noexcept
{
  if constexpr (std::is_signed_v<T>) {
    // positive overflow: both positive and sum negative
    if (rhs > 0 && lhs > std::numeric_limits<T>::max() - rhs) return true;
    // negative overflow: both negative and sum positive
    if (rhs < 0 && lhs < std::numeric_limits<T>::min() - rhs) return true;
    return false;
  } else {
    return lhs > std::numeric_limits<T>::max() - rhs;
  }
}

// Returns true if lhs - rhs overflows.
template<std::integral T>
[[nodiscard]] constexpr bool sub_overflows(T lhs, T rhs) noexcept
{
  if constexpr (std::is_signed_v<T>) {
    if (rhs < 0 && lhs > std::numeric_limits<T>::max() + rhs) return true;
    if (rhs > 0 && lhs < std::numeric_limits<T>::min() + rhs) return true;
    return false;
  } else {
    return lhs < rhs;
  }
}

// Returns true if lhs * rhs overflows using double-width arithmetic.
template<std::integral T>
[[nodiscard]] constexpr bool mul_overflows(T lhs, T rhs) noexcept
{
  using wide = double_width_int_for_t<T>;
  const wide product = static_cast<wide>(lhs) * static_cast<wide>(rhs);
  return product > static_cast<wide>(std::numeric_limits<T>::max()) ||
         product < static_cast<wide>(std::numeric_limits<T>::min());
}

// Returns true if lhs / rhs overflows (only INT_MIN / -1 for signed, or divide-by-zero).
template<std::integral T>
[[nodiscard]] constexpr bool div_overflows(T lhs, T rhs) noexcept
{
  if (rhs == 0) return true;
  if constexpr (std::is_signed_v<T>) {
    return lhs == std::numeric_limits<T>::min() && rhs == T{-1};
  }
  return false;
}

// Returns true if -lhs overflows (only INT_MIN for signed).
template<std::integral T>
[[nodiscard]] constexpr bool neg_overflows(T v) noexcept
{
  if constexpr (std::is_signed_v<T>) {
    return v == std::numeric_limits<T>::min();
  }
  return v != T{0};  // negation of any non-zero unsigned overflows
}

// Extracts the underlying integral type from an arithmetic wrapper:
//   - plain integral T                              → T
//   - integral wrapper with value_type (safe_int<T>, constrained<T,...>) → T::value_type
//   - non-arithmetic types (std::string, etc.)     → T (fails std::integral check later)
//
// Guarded by std::numeric_limits<T>::is_specialized to correctly exclude containers like
// std::string, whose value_type is char even though std::string is not an arithmetic type.
//
// Uses a type-membership check (`typename T::value_type`) rather than a function-call
// requires-expression (`{ v.value() }`) to avoid over-eager template instantiation in
// Clang 16, which would cause recursive satisfaction of `convertible_to` constraints.
template<typename T>
struct underlying_int_type_helper {
  using type = T;
};
template<typename T>
  requires requires { typename T::value_type; } && std::integral<typename T::value_type> &&
           std::numeric_limits<T>::is_specialized
struct underlying_int_type_helper<T> {
  using type = typename T::value_type;
};
template<typename T>
using underlying_int_type_t = typename underlying_int_type_helper<T>::type;

// True iff every value of From is exactly representable in To.
// Works for plain integrals and integral-valued wrappers with `value_type` (e.g. safe_int<T>).
template<typename From, typename To>
inline constexpr bool is_value_preserving_int_v = [] {
  using from_raw_t = underlying_int_type_t<From>;
  using to_raw_t = underlying_int_type_t<To>;
  if constexpr (std::integral<from_raw_t> && std::integral<to_raw_t>)
    return std::in_range<to_raw_t>(std::numeric_limits<from_raw_t>::min()) &&
           std::in_range<to_raw_t>(std::numeric_limits<from_raw_t>::max());
  else
    return false;
}();

// Generalized: uses integer range check when both types have integral underlying types,
// otherwise falls back to is_convertible.
template<typename From, typename To>
inline constexpr bool is_value_preserving_v = [] {
  if constexpr (std::integral<underlying_int_type_t<From>> && std::integral<underlying_int_type_t<To>>)
    return is_value_preserving_int_v<From, To>;
  else
    return std::is_convertible_v<From, To>;
}();

// Overflow-checked cast: raises via EP if integral value doesn't fit in To, then silently converts.
template<std::integral To, typename EP, typename From>
  requires std::is_constructible_v<To, const From&>
[[nodiscard]] constexpr To checked_int_cast(const From& v)
{
  if constexpr (std::integral<std::remove_cvref_t<From>>) {
    if (!std::in_range<To>(v)) EP::on_overflow("safe_int: narrowing conversion overflow");
  }
  return silent_cast<To>(v);
}

}  // namespace detail

// Forward declaration — needed by safe_int_binary_ops below.
MP_UNITS_EXPORT template<std::integral T, OverflowPolicy EP>
class safe_int;

// ============================================================================
// detail::safe_int_binary_ops — heterogeneous binary operators base class
//
// All binary operators are defined here as hidden friends using the Hidden Friend
// Injection Idiom (see constrained.h for rationale). This base class is inherited
// by safe_int<T,EP> to inject operators into the surrounding namespace.
//
// OPERATOR CATEGORIES (for each operator +, -, *, /, %):
//   1. safe_int × safe_int      → safe_int<promoted, EP>
//   2. safe_int × scalar:
//      a. Integral scalar       → safe_int<promoted, EP>     (overflow checks, wrapper preserved)
//      b. Non-integral scalar   → bare result type           (wrapper dropped)
//   3. safe_int × constrained:
//      a. Integral result       → safe_int<promoted, EP>     (overflow is tighter, safe_int wins)
//      b. Non-integral result   → constrained<promoted, CP>  (constraint wins, no overflow risk)
//   4. Comparisons              → all combinations (safe_int, scalar, constrained)
//
// DISPATCH MECHANISM:
//   - Partial constraint ordering: `requires std::integral<U>` overloads win when U is integral
//   - Unconstrained fallbacks handle non-integral types (FP, custom numeric types)
//
// WHY THIS DESIGN?
//   - Integral operations can overflow → safe_int provides overflow detection
//   - FP operations don't overflow in the same sense → wrapper can be safely dropped
//   - For cross-wrapper: integral results need overflow checks, non-integral preserve constraints
// ============================================================================

namespace detail {

struct safe_int_binary_ops {
  // ========================================================================
  // operator+ (addition)
  // ========================================================================

  // 1. safe_int × safe_int
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator+(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
    -> safe_int<decltype(lhs.value() + rhs.value()), EP>
  {
    using R = decltype(lhs.value() + rhs.value());
    if (detail::add_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: addition overflow");
    return lhs.value() + rhs.value();
  }

  // 2a. safe_int × integral scalar (wrapper preserved)
  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator+(const safe_int<T, EP>& lhs, const U& rhs)
    -> safe_int<decltype(lhs.value() + rhs), EP>
  {
    using R = decltype(lhs.value() + rhs);
    if (detail::add_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs)))
      EP::on_overflow("safe_int: addition overflow");
    return lhs.value() + rhs;
  }

  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator+(const U& lhs, const safe_int<T, EP>& rhs)
    -> safe_int<decltype(lhs + rhs.value()), EP>
  {
    using R = decltype(lhs + rhs.value());
    if (detail::add_overflows<R>(static_cast<R>(lhs), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: addition overflow");
    return lhs + rhs.value();
  }

  // 2b. safe_int × non-integral scalar (wrapper dropped)
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator+(const safe_int<T, EP>& lhs, const U& rhs) -> decltype(lhs.value() + rhs)
  {
    return lhs.value() + rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator+(const U& lhs, const safe_int<T, EP>& rhs) -> decltype(lhs + rhs.value())
  {
    return lhs + rhs.value();
  }

  // 3a. safe_int × constrained (integral result → safe_int wins)
  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator+(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> safe_int<decltype(lhs.value() + rhs.value()), EP>
  {
    return lhs.value() + rhs;
  }

  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator+(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> safe_int<decltype(lhs.value() + rhs.value()), EP>
  {
    return lhs + rhs.value();
  }

  // 3b. safe_int × constrained (non-integral result → constrained wins)
  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator+(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> constrained<decltype(lhs.value() + rhs.value()), CP>
  {
    return lhs + rhs.value();
  }

  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator+(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> constrained<decltype(lhs.value() + rhs.value()), CP>
  {
    return lhs.value() + rhs;
  }

  // ========================================================================
  // operator- (subtraction)
  // ========================================================================

  // 1. safe_int × safe_int
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator-(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
    -> safe_int<decltype(lhs.value() - rhs.value()), EP>
  {
    using R = decltype(lhs.value() - rhs.value());
    if (detail::sub_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: subtraction overflow");
    return lhs.value() - rhs.value();
  }

  // 2a. safe_int × integral scalar (wrapper preserved)
  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator-(const safe_int<T, EP>& lhs, const U& rhs)
    -> safe_int<decltype(lhs.value() - rhs), EP>
  {
    using R = decltype(lhs.value() - rhs);
    if (detail::sub_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs)))
      EP::on_overflow("safe_int: subtraction overflow");
    return lhs.value() - rhs;
  }

  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator-(const U& lhs, const safe_int<T, EP>& rhs)
    -> safe_int<decltype(lhs - rhs.value()), EP>
  {
    using R = decltype(lhs - rhs.value());
    if (detail::sub_overflows<R>(static_cast<R>(lhs), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: subtraction overflow");
    return lhs - rhs.value();
  }

  // 2b. safe_int × non-integral scalar (wrapper dropped)
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator-(const safe_int<T, EP>& lhs, const U& rhs) -> decltype(lhs.value() - rhs)
  {
    return lhs.value() - rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator-(const U& lhs, const safe_int<T, EP>& rhs) -> decltype(lhs - rhs.value())
  {
    return lhs - rhs.value();
  }

  // 3a. safe_int × constrained (integral result → safe_int wins)
  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator-(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> safe_int<decltype(lhs.value() - rhs.value()), EP>
  {
    return lhs.value() - rhs;
  }

  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator-(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> safe_int<decltype(lhs.value() - rhs.value()), EP>
  {
    return lhs - rhs.value();
  }

  // 3b. safe_int × constrained (non-integral result → constrained wins)
  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator-(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> constrained<decltype(lhs.value() - rhs.value()), CP>
  {
    return lhs - rhs.value();
  }

  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator-(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> constrained<decltype(lhs.value() - rhs.value()), CP>
  {
    return lhs.value() - rhs;
  }

  // ========================================================================
  // operator* (multiplication)
  // ========================================================================

  // 1. safe_int × safe_int
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
    -> safe_int<decltype(lhs.value() * rhs.value()), EP>
  {
    using R = decltype(lhs.value() * rhs.value());
    if (detail::mul_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: multiplication overflow");
    return lhs.value() * rhs.value();
  }

  // 2a. safe_int × integral scalar (wrapper preserved)
  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator*(const safe_int<T, EP>& lhs, const U& rhs)
    -> safe_int<decltype(lhs.value() * rhs), EP>
  {
    using R = decltype(lhs.value() * rhs);
    if (detail::mul_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs)))
      EP::on_overflow("safe_int: multiplication overflow");
    return lhs.value() * rhs;
  }

  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator*(const U& lhs, const safe_int<T, EP>& rhs)
    -> safe_int<decltype(lhs * rhs.value()), EP>
  {
    using R = decltype(lhs * rhs.value());
    if (detail::mul_overflows<R>(static_cast<R>(lhs), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: multiplication overflow");
    return lhs * rhs.value();
  }

  // 2b. safe_int × non-integral scalar (wrapper dropped)
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const safe_int<T, EP>& lhs, const U& rhs) -> decltype(lhs.value() * rhs)
  {
    return lhs.value() * rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator*(const U& lhs, const safe_int<T, EP>& rhs) -> decltype(lhs * rhs.value())
  {
    return lhs * rhs.value();
  }

  // 3a. safe_int × constrained (integral result → safe_int wins)
  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator*(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> safe_int<decltype(lhs.value() * rhs.value()), EP>
  {
    return lhs.value() * rhs;
  }

  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator*(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> safe_int<decltype(lhs.value() * rhs.value()), EP>
  {
    return lhs * rhs.value();
  }

  // 3b. safe_int × constrained (non-integral result → constrained wins)
  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator*(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> constrained<decltype(lhs.value() * rhs.value()), CP>
  {
    return lhs * rhs.value();
  }

  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator*(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> constrained<decltype(lhs.value() * rhs.value()), CP>
  {
    return lhs.value() * rhs;
  }

  // ========================================================================
  // operator/ (division)
  // ========================================================================

  // 1. safe_int × safe_int
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
    -> safe_int<decltype(lhs.value() / rhs.value()), EP>
  {
    using R = decltype(lhs.value() / rhs.value());
    if (detail::div_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: division overflow");
    return lhs.value() / rhs.value();
  }

  // 2a. safe_int × integral scalar (wrapper preserved)
  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator/(const safe_int<T, EP>& lhs, const U& rhs)
    -> safe_int<decltype(lhs.value() / rhs), EP>
  {
    using R = decltype(lhs.value() / rhs);
    if (detail::div_overflows<R>(static_cast<R>(lhs.value()), static_cast<R>(rhs)))
      EP::on_overflow("safe_int: division overflow");
    return lhs.value() / rhs;
  }

  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator/(const U& lhs, const safe_int<T, EP>& rhs)
    -> safe_int<decltype(lhs / rhs.value()), EP>
  {
    using R = decltype(lhs / rhs.value());
    if (detail::div_overflows<R>(static_cast<R>(lhs), static_cast<R>(rhs.value())))
      EP::on_overflow("safe_int: division overflow");
    return static_cast<R>(lhs) / static_cast<R>(rhs.value());
  }

  // 2b. safe_int × non-integral scalar (wrapper dropped)
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const safe_int<T, EP>& lhs, const U& rhs) -> decltype(lhs.value() / rhs)
  {
    return lhs.value() / rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator/(const U& lhs, const safe_int<T, EP>& rhs) -> decltype(lhs / rhs.value())
  {
    return lhs / static_cast<decltype(lhs / rhs.value())>(rhs.value());
  }

  // 3a. safe_int × constrained (integral result → safe_int wins)
  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator/(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> safe_int<decltype(lhs.value() / rhs.value()), EP>
  {
    return lhs.value() / rhs;
  }

  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator/(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> safe_int<decltype(lhs.value() / rhs.value()), EP>
  {
    return lhs / rhs.value();
  }

  // 3b. safe_int × constrained (non-integral result → constrained wins)
  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator/(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> constrained<decltype(lhs.value() / rhs.value()), CP>
  {
    return lhs / rhs.value();
  }

  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator/(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> constrained<decltype(lhs.value() / rhs.value()), CP>
  {
    return lhs.value() / rhs;
  }

  // ========================================================================
  // operator% (modulo) — integral only (% undefined for non-integral)
  // ========================================================================

  // 1. safe_int × safe_int
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator%(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
    -> safe_int<decltype(lhs.value() % rhs.value()), EP>
  {
    if (rhs.value() == U{0}) EP::on_overflow("safe_int: modulo by zero");
    return lhs.value() % rhs.value();
  }

  // 2a. safe_int × integral scalar (wrapper preserved)
  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator%(const safe_int<T, EP>& lhs, const U& rhs)
    -> safe_int<decltype(lhs.value() % rhs), EP>
  {
    if (rhs == U{0}) EP::on_overflow("safe_int: modulo by zero");
    return lhs.value() % rhs;
  }

  template<typename T, typename EP, typename U>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator%(const U& lhs, const safe_int<T, EP>& rhs)
    -> safe_int<decltype(lhs % rhs.value()), EP>
  {
    if (rhs.value() == T{0}) EP::on_overflow("safe_int: modulo by zero");
    return lhs % rhs.value();
  }

  // 3a. safe_int × constrained (integral only → safe_int wins)
  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator%(constrained<U, CP> lhs, safe_int<T, EP> rhs)
    -> safe_int<decltype(lhs.value() % rhs.value()), EP>
  {
    return lhs.value() % rhs;
  }

  template<typename T, typename EP, typename U, typename CP>
    requires std::integral<U>
  [[nodiscard]] friend constexpr auto operator%(safe_int<T, EP> lhs, constrained<U, CP> rhs)
    -> safe_int<decltype(lhs.value() % rhs.value()), EP>
  {
    return lhs % rhs.value();
  }

  // ========================================================================
  // Comparison operators (== and <=>)
  // C++20's rewrite rules provide all relational operators (<, <=, >, >=, !=)
  // from these two. All combinations of safe_int, scalar, and constrained.
  // ========================================================================
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr bool operator==(const safe_int<T, EP>& lhs, const safe_int<U, EP>& rhs)
  {
    return std::cmp_equal(lhs.value(), rhs.value());
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const safe_int<T, EP>& lhs,
                                                                  const safe_int<U, EP>& rhs)
  {
    if (std::cmp_less(lhs.value(), rhs.value())) return std::strong_ordering::less;
    if (std::cmp_greater(lhs.value(), rhs.value())) return std::strong_ordering::greater;
    return std::strong_ordering::equal;
  }

  // safe_int vs scalar (integral — uses std::cmp_* for signed/unsigned safety)
  template<typename T, typename EP, std::integral U>
  [[nodiscard]] friend constexpr bool operator==(const safe_int<T, EP>& lhs, const U& rhs)
  {
    return std::cmp_equal(lhs.value(), rhs);
  }

  template<typename T, typename EP, std::integral U>
  [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const safe_int<T, EP>& lhs, const U& rhs)
  {
    if (std::cmp_less(lhs.value(), rhs)) return std::strong_ordering::less;
    if (std::cmp_greater(lhs.value(), rhs)) return std::strong_ordering::greater;
    return std::strong_ordering::equal;
  }

  // safe_int vs scalar (non-integral fallback)
  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr bool operator==(const safe_int<T, EP>& lhs, const U& rhs)
  {
    return lhs.value() == rhs;
  }

  template<typename T, typename EP, typename U>
  [[nodiscard]] friend constexpr auto operator<=>(const safe_int<T, EP>& lhs, const U& rhs)
  {
    return lhs.value() <=> rhs;
  }

  // safe_int vs constrained
  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr bool operator==(const safe_int<T, EP>& lhs, const constrained<U, CP>& rhs)
  {
    return lhs.value() == rhs.value();
  }

  template<typename T, typename EP, typename U, typename CP>
  [[nodiscard]] friend constexpr auto operator<=>(const safe_int<T, EP>& lhs, const constrained<U, CP>& rhs)
  {
    return lhs.value() <=> rhs.value();
  }
};

}  // namespace detail

// ============================================================================
// safe_int<T, ErrorPolicy>
// ============================================================================

/**
 * @brief Wraps an integral type with overflow detection.
 *
 * This class wraps any integral type and models all requirements for mp-units
 * representation types (RealScalar, MagnitudeScalable, etc.). Every arithmetic
 * operation checks for overflow and delegates to the ErrorPolicy::on_overflow()
 * handler on detection.
 *
 * @tparam T            the underlying integral type (e.g. int, long, uint32_t)
 * @tparam ErrorPolicy  how to react to overflow — default: safe_int_throw_policy on hosted,
 *                      safe_int_terminate_policy on freestanding
 */
MP_UNITS_EXPORT template<std::integral T,
#if MP_UNITS_HOSTED
                         OverflowPolicy ErrorPolicy = safe_int_throw_policy>
#else
                         OverflowPolicy ErrorPolicy = safe_int_terminate_policy>
#endif
class safe_int : detail::safe_int_binary_ops {
  // Invoke ErrorPolicy and return a safe default (for use from noexcept compound-assign paths).
  static constexpr void handle_overflow(std::string_view msg) { ErrorPolicy::on_overflow(msg); }
public:
  // public members required to satisfy structural type requirements :-(
  T value_{};
  using value_type = T;
  using error_policy = ErrorPolicy;

  safe_int() = default;

  // Converting constructor from any non-floating-point, non-safe_int type constructible as T
  // Explicit when the source type cannot represent all values of T (narrowing)
  template<typename U>
    requires(!treat_as_floating_point<std::remove_cvref_t<U>>) && std::is_constructible_v<T, U>
  constexpr explicit(!detail::is_value_preserving_v<std::remove_cvref_t<U>, T>) safe_int(const U& v) :
      value_(detail::checked_int_cast<T, ErrorPolicy>(v))
  {
  }

  // Converting constructor from safe_int<U> with matching error policy
  // Implicit when U→T is value-preserving (every U value fits in T), explicit otherwise
  template<std::integral U>
    requires(!std::is_same_v<T, U>) && std::is_constructible_v<T, U>
  constexpr explicit(!detail::is_value_preserving_int_v<U, T>) safe_int(const safe_int<U, ErrorPolicy>& other) :
      value_(detail::checked_int_cast<T, ErrorPolicy>(other.value()))
  {
  }

  // Implicit conversion to the underlying type allows passing safe_int to legacy
  // interfaces that accept raw integral types without an explicit .value() or cast.
  [[nodiscard]] constexpr explicit(false) operator T() const noexcept { return value_; }
  [[nodiscard]] constexpr T value() const noexcept { return value_; }

  // ==========================================================================
  // Unary operators (+, -, ++, --)
  // Models integral promotion: sub-int types (char, short) promote to int.
  // ==========================================================================

  // -- Unary arithmetic --
  [[nodiscard]] constexpr auto operator+() const -> safe_int<decltype(+value_), ErrorPolicy> { return +value_; }
  [[nodiscard]] constexpr auto operator-() const -> safe_int<decltype(-value_), ErrorPolicy>
  {
    if (detail::neg_overflows(+value_)) handle_overflow("safe_int: negation overflow");
    return -value_;
  }

  // -- Increment / decrement (use add/sub overflow check when T is integral) --
  constexpr safe_int& operator++()
  {
    if (detail::add_overflows(value_, T{1})) handle_overflow("safe_int: increment overflow");
    ++value_;
    return *this;
  }

  constexpr safe_int operator++(int)
  {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  constexpr safe_int& operator--()
  {
    if (detail::sub_overflows(value_, T{1})) handle_overflow("safe_int: decrement overflow");
    --value_;
    return *this;
  }

  constexpr safe_int operator--(int)
  {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  // ==========================================================================
  // Compound assignment operators (+=, -=, *=, /=, %=)
  // Only accept safe_int<T,EP> rhs — no implicit scalar conversions to preserve
  // explicit overflow semantics. Use binary operators for mixed-type operations.
  // ==========================================================================

  // -- Compound assignment --
  constexpr safe_int& operator+=(const safe_int& rhs)
  {
    if (detail::add_overflows(value_, rhs.value_)) handle_overflow("safe_int: addition overflow");
    value_ += rhs.value_;
    return *this;
  }

  constexpr safe_int& operator-=(const safe_int& rhs)
  {
    if (detail::sub_overflows(value_, rhs.value_)) handle_overflow("safe_int: subtraction overflow");
    value_ -= rhs.value_;
    return *this;
  }

  constexpr safe_int& operator*=(const safe_int& rhs)
  {
    if (detail::mul_overflows(value_, rhs.value_)) handle_overflow("safe_int: multiplication overflow");
    value_ *= rhs.value_;
    return *this;
  }

  constexpr safe_int& operator/=(const safe_int& rhs)
  {
    if (detail::div_overflows(value_, rhs.value_)) handle_overflow("safe_int: division overflow");
    value_ /= rhs.value_;
    return *this;
  }

  constexpr safe_int& operator%=(const safe_int& rhs)
  {
    if (rhs.value_ == T{0}) handle_overflow("safe_int: modulo by zero");
    value_ %= rhs.value_;
    return *this;
  }
};

template<std::integral T>
safe_int(T) -> safe_int<T>;

template<typename T, typename ErrorPolicy>
struct constraint_violation_handler<safe_int<T, ErrorPolicy>> {
  static constexpr void on_violation(std::string_view msg) { ErrorPolicy::on_constraint_violation(msg); }
};

// ============================================================================
// Convenience type aliases
// ============================================================================

#if MP_UNITS_HOSTED
// Hosted environment: default to throw_policy
MP_UNITS_EXPORT using safe_i8 = safe_int<std::int8_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_i16 = safe_int<std::int16_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_i32 = safe_int<std::int32_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_i64 = safe_int<std::int64_t, safe_int_throw_policy>;

MP_UNITS_EXPORT using safe_u8 = safe_int<std::uint8_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_u16 = safe_int<std::uint16_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_u32 = safe_int<std::uint32_t, safe_int_throw_policy>;
MP_UNITS_EXPORT using safe_u64 = safe_int<std::uint64_t, safe_int_throw_policy>;
#else
// Freestanding environment: default to terminate_policy
MP_UNITS_EXPORT using safe_i8 = safe_int<std::int8_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_i16 = safe_int<std::int16_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_i32 = safe_int<std::int32_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_i64 = safe_int<std::int64_t, safe_int_terminate_policy>;

MP_UNITS_EXPORT using safe_u8 = safe_int<std::uint8_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_u16 = safe_int<std::uint16_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_u32 = safe_int<std::uint32_t, safe_int_terminate_policy>;
MP_UNITS_EXPORT using safe_u64 = safe_int<std::uint64_t, safe_int_terminate_policy>;
#endif

}  // namespace mp_units

// std::numeric_limits specialization — required for representation_values<safe_int<T>>
namespace std {

template<typename T, typename ErrorPolicy>
class numeric_limits<mp_units::safe_int<T, ErrorPolicy>> : public numeric_limits<T> {
  using S = mp_units::safe_int<T, ErrorPolicy>;

public:
  [[nodiscard]] static constexpr S lowest() noexcept { return numeric_limits<T>::lowest(); }
  [[nodiscard]] static constexpr S min() noexcept { return numeric_limits<T>::min(); }
  [[nodiscard]] static constexpr S max() noexcept { return numeric_limits<T>::max(); }
  [[nodiscard]] static constexpr S epsilon() noexcept { return numeric_limits<T>::epsilon(); }
  [[nodiscard]] static constexpr S round_error() noexcept { return numeric_limits<T>::round_error(); }
  [[nodiscard]] static constexpr S infinity() noexcept { return numeric_limits<T>::infinity(); }
  [[nodiscard]] static constexpr S quiet_NaN() noexcept { return numeric_limits<T>::quiet_NaN(); }
  [[nodiscard]] static constexpr S signaling_NaN() noexcept { return numeric_limits<T>::signaling_NaN(); }
  [[nodiscard]] static constexpr S denorm_min() noexcept { return numeric_limits<T>::denorm_min(); }
};

}  // namespace std

#if MP_UNITS_HOSTED
template<typename T, typename ErrorPolicy, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::safe_int<T, ErrorPolicy>, Char> : formatter<T, Char> {
  template<typename FormatContext>
  auto format(const mp_units::safe_int<T, ErrorPolicy>& v, FormatContext& ctx) const
  {
    return formatter<T, Char>::format(v.value(), ctx);
  }
};
#endif

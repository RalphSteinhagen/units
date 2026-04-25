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
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/point_origin_concepts.h>
#include <mp-units/framework/quantity_character.h>
#include <mp-units/framework/quantity_spec_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <limits>
#include <string_view>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <chrono>
#endif
#endif
#endif

namespace mp_units {

namespace detail {

/**
 * @brief Sentinel type indicating no default implementation for a variable template
 *
 * This type is used as a placeholder for primary variable templates that should not
 * have a default implementation, working around the language limitation that variable
 * templates cannot be "deleted" like functions can.
 */
struct undefined_t {};

inline constexpr undefined_t undefined{};

template<typename>
struct cond_underlying_type {};

template<typename T>
  requires std::is_object_v<T>
struct cond_underlying_type<T> {
  using type = std::remove_cv_t<T>;
};

template<typename T>
concept has_value_type_member = requires { typename T::value_type; };

template<typename T>
concept has_element_type_member = requires { typename T::element_type; };

}  // namespace detail

/**
 * @brief Describes the underlying arithmetic/element type of a representation type.
 *
 * The library uses this trait to discover the "scalar element" inside a representation
 * type (e.g. `int` for `safe_int<int>`, `double` for `cartesian_vector<double>`). It is
 * used to drive classification (integer vs floating-point scaling path, default of
 * `treat_as_floating_point`, etc.) — it is **not** an indirection/dereference trait.
 *
 * The primary template is intentionally empty so that types which do not expose an
 * underlying type are cleanly recognized as "leaf" representations (the recursive
 * unwrapping in @c detail::value_type_t stops at them).
 *
 * Default detection (via partial specializations below) follows the same shape as the
 * standard's `std::indirectly_readable_traits` for its `value_type` / `element_type`
 * cases (see [readable.traits]): nested `::value_type`, else nested `::element_type`;
 * a top-level `const` is passed through; the detected alias has its cv-qualification
 * removed; and if a type provides both `::value_type` and `::element_type` whose
 * underlying types differ after ignoring top-level cv-qualification, the trait is
 * empty (the user must disambiguate explicitly). Pointer and array specializations
 * from the standard are intentionally omitted — those are modeled by the standard's
 * iterator machinery, not by this trait. Additionally, scoped enumeration types are
 * supported via `std::underlying_type_t` — a representation-model extension not
 * present in the standard's iterator-oriented trait (unscoped enums are excluded
 * because they already implicitly convert to their underlying type).
 *
 * @tparam T the representation type
 */
MP_UNITS_EXPORT template<typename T>
struct representation_underlying_type {};

template<typename T>
struct representation_underlying_type<const T> : representation_underlying_type<T> {};

template<typename T>
  requires std::is_scoped_enum_v<T>
struct representation_underlying_type<T> {
  using type = std::underlying_type_t<T>;
};

template<detail::has_value_type_member T>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::value_type> {};

template<detail::has_element_type_member T>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::element_type> {};

template<detail::has_value_type_member T>
  requires detail::has_element_type_member<T>
struct representation_underlying_type<T> {};

template<detail::has_value_type_member T>
  requires detail::has_element_type_member<T> &&
           std::same_as<std::remove_cv_t<typename T::value_type>, std::remove_cv_t<typename T::element_type>>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::value_type> {};

MP_UNITS_EXPORT template<typename T>
  requires requires { typename representation_underlying_type<T>::type; }
using representation_underlying_type_t = typename representation_underlying_type<T>::type;

namespace detail {

template<typename T>
struct value_type_impl {
  using type = T;
};

template<typename T>
  requires requires { typename representation_underlying_type_t<T>; }
struct value_type_impl<T> {
  using type = value_type_impl<representation_underlying_type_t<T>>::type;
};

template<typename T>
  requires std::is_object_v<T>
using value_type_t = value_type_impl<T>::type;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Specifies if a value of a type should be treated as a floating-point value
 *
 * This type trait should be specialized for a custom representation type to specify
 * that values of this type should be treated by the library as a floating-point ones
 * which will enable implicit conversions between quantities.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
constexpr bool treat_as_floating_point =
#if MP_UNITS_HOSTED
  std::chrono::treat_as_floating_point_v<detail::value_type_t<Rep>>;
#else
  std::is_floating_point_v<detail::value_type_t<Rep>>;
#endif

/**
 * @brief Controls whether conversion from `quantity<FromUnit, FromRep>` to
 *        `quantity<ToUnit, ToRep>` is implicit or explicit.
 *
 * The default is `true` iff `FromRep` is convertible to `ToRep` and the scaling is
 * non-truncating: either `ToRep` is floating-point, or both reps are non-floating-point
 * and the unit magnitude ratio is an integral factor.
 *
 * Specialize this variable template to customize the implicit/explicit decision for
 * your own representation types.
 *
 * @note The default initializer references `is_integral_scaling`, which is defined in
 *       `<mp-units/framework/value_cast.h>`. The unqualified call is found via ADL at
 *       instantiation time, so any translation unit that instantiates this template must
 *       have `value_cast.h` (or a header transitively including it) included.
 *
 * @tparam FromUnit the source unit value (NTTP)
 * @tparam FromRep  the source representation type
 * @tparam ToUnit   the target unit value (NTTP)
 * @tparam ToRep    the target representation type
 */
template<auto FromUnit, typename FromRep, auto ToUnit, typename ToRep>
constexpr bool implicitly_scalable =
  treat_as_floating_point<ToRep> || (!treat_as_floating_point<FromRep> && is_integral_scaling(FromUnit, ToUnit));

/**
 * @brief Specifies if a specific conversion between two types is representable without data loss
 *
 * @deprecated Use `mp_units::implicitly_scalable` instead.
 *
 * @tparam From a source representation type
 * @tparam To a destination representation type
 */
template<typename From, typename To>
[[deprecated("2.6.0: Use `mp_units::implicitly_scalable` instead")]]
constexpr bool is_value_preserving = treat_as_floating_point<To> || !treat_as_floating_point<From>;

template<typename Rep>
[[deprecated("2.5.0: `is_scalar` is no longer necessary and can simply be removed")]]
constexpr bool is_scalar = false;

template<typename Rep>
[[deprecated("2.5.0: `is_complex` is no longer necessary and can simply be removed")]]
constexpr bool is_complex = false;

template<typename Rep>
[[deprecated("2.5.0: `is_vector` is no longer necessary and can simply be removed")]]
constexpr bool is_vector = false;

template<typename Rep>
[[deprecated("2.5.0: `is_tensor` is no longer necessary and can simply be removed")]]
constexpr bool is_tensor = false;

/**
 * @brief A type trait that defines zero, one, min, and max for a representation type
 *
 * The zero, one, min, and max member functions in mp_units::quantity forward their work to
 * these methods. This type can be specialized if the representation Rep requires a specific
 * implementation to return these quantity objects.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
#if MP_UNITS_HOSTED
struct representation_values : std::chrono::duration_values<Rep> {
#else
struct representation_values {
  static constexpr Rep zero() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(0);
  }

  static constexpr Rep min() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::lowest() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::lowest();
  }

  static constexpr Rep max() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::max() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::max();
  }
#endif

  static constexpr Rep one() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(1);
  }
};

template<typename Rep>
using quantity_values [[deprecated("2.5.0: Use `representation_values` instead")]] = representation_values<Rep>;

/**
 * @brief Customization point for providing bounds on a quantity point relative to an origin.
 *
 * Users specialize this variable template for point origins (both absolute and relative).
 * The value should be an overflow-policy object that stores the bounds
 * (e.g. `reflect_in_range`, `clamp_to_range`, `wrap_to_range`).
 *
 * Bounds are always enforced as constraints on `quantity_from(Origin)` — the displacement
 * measured in the frame of the specialized origin. Different origins (including relative
 * ones) may have independent bounds; it is the user's responsibility to keep them
 * consistent when multiple levels of bounds are in use.
 *
 * Example:
 * @code{.cpp}
 * // Absolute origin: latitude reflects at poles
 * inline constexpr struct equator final : absolute_point_origin<geo_latitude> {} equator;
 * template<>
 * constexpr auto quantity_bounds<equator> = reflect_in_range{-90 * deg, 90 * deg};
 *
 * // Relative origin: AC controller setpoint — comfort range clamped around 21 °C
 * inline constexpr struct room_reference_temp final :
 *     relative_point_origin<point<deg_C>(21)> {} room_reference_temp;
 * template<>
 * constexpr auto quantity_bounds<room_reference_temp> = clamp_to_range{delta<deg_C>(-3), delta<deg_C>(3)};
 * @endcode
 *
 * @tparam PO a point origin for which bounds are defined
 */
template<PointOrigin auto PO>
  requires(PO._quantity_spec_.character == quantity_character::real_scalar)
inline constexpr auto quantity_bounds = detail::quantity_bounds_for<MP_UNITS_REMOVE_CONST(decltype(PO))>;

MP_UNITS_EXPORT_END

/**
 * @brief Customization point for handling domain constraint violations.
 *
 * The library calls `constraint_violation_handler<Rep>::on_violation(msg)` when
 * a domain constraint is violated (e.g., out-of-bounds value, non-negativity).
 *
 * No default implementation is provided. When no specialization exists for a given
 * Rep, the library uses MP_UNITS_EXPECTS directly (which may compile to nothing in
 * release builds). Specializing this trait opts the type into guaranteed enforcement.
 *
 * @tparam Rep the representation type
 */
MP_UNITS_EXPORT template<typename Rep>
struct constraint_violation_handler;

namespace detail {

template<typename Rep>
concept HasConstraintViolationHandler =
  requires(std::string_view msg) { constraint_violation_handler<Rep>::on_violation(msg); };

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Provides support for external quantity-like types
 *
 * The type trait should provide the @c reference object, a type alias @c rep,
 * and static member functions @c to_numerical_value(T) that returns the raw value
 * of the quantity and @c from_numerical_value(rep) that returns @c T from @c rep.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity type,
 * - @c explicit_export for the conversion from a @c quantity type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_like_traits;

/**
 * @brief Provides support for external quantity point-like types
 *
 * The type trait should provide nested @c reference and @c origin objects,
 * a type alias @c rep, and static member functions @c to_numerical_value(T) that
 * returns the raw value of the the quantity being the offset of the point from the
 * origin and @c from_numerical_value(rep) that returns @c T formed this raw value.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity_point type,
 * - @c explicit_export for the conversion from a @c quantity_point type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_point_like_traits;

template<typename T>
constexpr auto unit_for = detail::undefined;

template<typename T>
  requires requires { quantity_like_traits<T>::reference; }
constexpr auto unit_for<T> = get_unit(quantity_like_traits<T>::reference);  // has to be unqualified for late binding

template<typename T>
  requires requires { quantity_point_like_traits<T>::reference; }
constexpr auto unit_for<T> =
  get_unit(quantity_point_like_traits<T>::reference);  // has to be unqualified for late binding

template<typename T>
constexpr auto reference_for = detail::undefined;

template<typename T>
  requires requires { quantity_like_traits<T>::reference; }
constexpr auto reference_for<T> = quantity_like_traits<T>::reference;

template<typename T>
  requires requires { quantity_point_like_traits<T>::reference; }
constexpr auto reference_for<T> = quantity_point_like_traits<T>::reference;

MP_UNITS_EXPORT_END

namespace detail {

template<typename T>
struct rep_for_impl {};

template<typename T>
  requires requires { typename quantity_like_traits<T>::rep; }
struct rep_for_impl<T> {
  using type = typename quantity_like_traits<T>::rep;
};

template<typename T>
  requires requires { typename quantity_point_like_traits<T>::rep; }
struct rep_for_impl<T> {
  using type = typename quantity_point_like_traits<T>::rep;
};

}  // namespace detail

MP_UNITS_EXPORT template<typename T>
using rep_for = typename detail::rep_for_impl<T>::type;

}  // namespace mp_units

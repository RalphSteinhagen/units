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

#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/contracts.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif

namespace mp_units {

// ============================================================================
// Bounds-checking and value-transformation policies
//
// Each policy is a class template parameterised on a quantity type Q.
// It stores the [min, max] bounds and provides operator()(V v) that
// enforces those bounds on a quantity of compatible type.
//
// Available policies:
//   1. check_in_range       - Error reporting via constraint_violation_handler or MP_UNITS_EXPECTS
//   2. clamp_to_range       - Saturate to boundaries (error correction)
//   3. wrap_to_range         - Modulo wrapping to [min, max)
//   4. reflect_in_range      - Bounce/fold at boundaries (physics)
//
// When to use:
//   - Use check_in_range for bounds-checked points (error behavior depends on the rep type)
//   - Use clamp_to_range when you want to "correct" out-of-range values
//   - Use wrap_to_range for periodic/cyclic values (angles, hours)
//   - Use reflect_in_range for physical boundaries (latitude, bouncing particles)
// ============================================================================

/**
 * @brief Policy that checks the value is within [min, max] and reports violations.
 *
 * If the quantity's representation type has a `constraint_violation_handler` specialization,
 * the handler's `on_violation()` is called on out-of-bounds values (providing guaranteed
 * enforcement regardless of build mode). Otherwise, falls back to `MP_UNITS_EXPECTS`,
 * which may be disabled in release builds.
 *
 * Example:
 * @code{cpp}
 * // With constrained<double, throw_policy> rep → throws std::domain_error on violation
 * // With plain double rep → asserts via MP_UNITS_EXPECTS (may be no-op in release)
 * template<>
 * constexpr auto quantity_bounds<equator> = check_in_range{-90 * deg, 90 * deg};
 * @endcode
 */
MP_UNITS_EXPORT template<Quantity Q>
struct check_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    if constexpr (detail::HasConstraintViolationHandler<typename V::rep>) {
      if (v < vmin || v > vmax) constraint_violation_handler<typename V::rep>::on_violation("value out of bounds");
    } else {
      MP_UNITS_EXPECTS(v >= vmin && v <= vmax);
    }
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
check_in_range(Q, Q) -> check_in_range<Q>;

#endif

/**
 * @brief Policy that clamps the value to [min, max].
 *
 * Saturates out-of-range values to the nearest boundary.
 * Use when you want to "correct" invalid values rather than signal an error.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct clamp_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    if (v < vmin) return vmin;
    if (v > vmax) return vmax;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
clamp_to_range(Q, Q) -> clamp_to_range<Q>;

#endif

/**
 * @brief Policy that wraps the value into the half-open range [min, max).
 *
 * Uses modulo arithmetic to wrap values into the range.
 * Use for periodic/cyclic quantities (angles, time-of-day, etc.).
 * For example, with [0°, 360°): 370° -> 10°, -10° -> 350°.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct wrap_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    const quantity range = vmax - vmin;
    while (v >= vmax) v -= range;
    while (v < vmin) v += range;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
wrap_to_range(Q, Q) -> wrap_to_range<Q>;

#endif

/**
 * @brief Policy that reflects (folds) the value at both boundaries.
 *
 * Values that exceed [min, max] are "bounced back" from the boundary.
 * For example, with [-90, 90] (latitude): 91 -> 89, 180 -> 0, 270 -> -90.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct reflect_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const V vmin{min};
    const V vmax{max};
    const quantity range = vmax - vmin;
    const quantity period = V{2 * range};
    while (v >= V{vmin + period}) v -= period;
    while (v < vmin) v += period;
    if (v > vmax) v = V{2 * vmax} - v;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
reflect_in_range(Q, Q) -> reflect_in_range<Q>;

#endif

}  // namespace mp_units

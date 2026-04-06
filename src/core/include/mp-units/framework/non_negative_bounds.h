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
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_point.h>
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/overflow_policies.h>

namespace mp_units {

// Partial specialization of `detail::quantity_bounds_for` for every
// `natural_point_origin_<QS>` whose quantity spec is tagged `non_negative` in the ISQ
// (e.g. length, mass, duration, thermodynamic_temperature).
//
// This file lives in a separate header (included last by <mp-units/framework.h>) because
// it bridges two headers that cannot include one another:
//   - natural_point_origin_  is defined in <mp-units/framework/quantity_point.h>
//   - is_non_negative()      is defined in <mp-units/framework/quantity_spec.h>
//   - check_non_negative     is defined in <mp-units/overflow_policies.h>
//

namespace detail {

template<QuantitySpec auto QS>
  requires(is_non_negative(QS))
inline constexpr auto quantity_bounds_for<natural_point_origin_<QS>> = check_non_negative{};

}  // namespace detail

}  // namespace mp_units

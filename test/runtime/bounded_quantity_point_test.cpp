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

#include <catch2/catch_test_macros.hpp>
#include <mp-units/constrained.h>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <stdexcept>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

#if MP_UNITS_HOSTED

// ============================================================================
// check_in_range tests — out-of-bounds violations must throw.
//
// Using constrained<double, throw_policy> as the rep ensures that
// constraint_violation_handler is called unconditionally (regardless of
// MP_UNITS_API_CONTRACTS), and that handler throws std::domain_error.
// This is the only behaviour that cannot be covered by static_assert tests.
// ============================================================================

namespace {

QUANTITY_SPEC(test_angle_check, isq::angular_measure);

inline constexpr struct check_origin final :
    absolute_point_origin<test_angle_check, check_in_range{-90 * deg, 90 * deg}> {
} check_origin;

using safe_double = constrained<double, throw_policy>;
using qp_check = quantity_point<test_angle_check[deg], check_origin, safe_double>;

}  // namespace

TEST_CASE("check_in_range throws on construction outside bounds", "[bounded][check]")
{
  CHECK_THROWS_AS(qp_check(91.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(-91.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(180.0 * test_angle_check[deg], check_origin), std::domain_error);
  CHECK_THROWS_AS(qp_check(-180.0 * test_angle_check[deg], check_origin), std::domain_error);
}

TEST_CASE("check_in_range does not throw within bounds", "[bounded][check]")
{
  CHECK_NOTHROW(qp_check(0.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(45.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(90.0 * test_angle_check[deg], check_origin));
  CHECK_NOTHROW(qp_check(-90.0 * test_angle_check[deg], check_origin));
}

TEST_CASE("check_in_range throws on arithmetic that crosses bounds", "[bounded][check]")
{
  SECTION("operator+= crosses max")
  {
    auto pt = qp_check(80.0 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(pt += 20.0 * test_angle_check[deg], std::domain_error);
  }

  SECTION("operator-= crosses min")
  {
    auto pt = qp_check(-80.0 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(pt -= 20.0 * test_angle_check[deg], std::domain_error);
  }

  SECTION("operator++ crosses max")
  {
    using qp_check_int = quantity_point<test_angle_check[deg], check_origin, constrained<int, throw_policy>>;
    auto pt = qp_check_int(90 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(++pt, std::domain_error);
  }

  SECTION("operator-- crosses min")
  {
    using qp_check_int = quantity_point<test_angle_check[deg], check_origin, constrained<int, throw_policy>>;
    auto pt = qp_check_int(-90 * test_angle_check[deg], check_origin);
    CHECK_THROWS_AS(--pt, std::domain_error);
  }
}

#endif  // MP_UNITS_HOSTED

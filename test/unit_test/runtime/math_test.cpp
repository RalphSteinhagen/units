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

#include "almost_equals.h"
#include <catch2/catch_all.hpp>
#include <units/isq/space_and_time.h>
#include <units/math.h>
#include <units/quantity_io.h>
#include <units/si/unit_symbols.h>
#include <units/si/units.h>
#include <limits>

using namespace units;
using namespace units::si::unit_symbols;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  SECTION("'pow<0>(q)' returns '1'") { CHECK(pow<0>(2 * isq::length[m]) == 1); }

  SECTION("'pow<1>(q)' returns 'q'") { CHECK(pow<1>(2 * isq::length[m]) == 2 * isq::length[m]); }

  SECTION("'pow<2>(q)' squares both the value and a dimension")
  {
    CHECK(pow<2>(2 * isq::length[m]) == 4 * isq::area[m2]);
  }

  SECTION("'pow<3>(q)' cubes both the value and a dimension")
  {
    CHECK(pow<3>(2 * isq::length[m]) == 8 * isq::volume[m3]);
  }
}

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4 * isq::area[m2]) == 2 * isq::length[m]);
}

TEST_CASE("'cbrt()' on quantity changes the value and the dimension accordingly", "[math][cbrt]")
{
  REQUIRE(cbrt(8 * isq::volume[m3]) == 2 * isq::length[m]);
}

TEST_CASE("'pow<Num, Den>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  REQUIRE(pow<1, 4>(16 * isq::area[m2]) == sqrt(4 * isq::length[m]));
}

TEST_CASE("absolute functions on quantity returns the absolute value", "[math][abs][fabs]")
{
  SECTION("'abs()' on a negative quantity returns the abs")
  {
    SECTION("integral representation") { REQUIRE(abs(-1 * isq::length[m]) == 1 * isq::length[m]); }

    SECTION("floating-point representation") { REQUIRE(abs(-1. * isq::length[m]) == 1 * isq::length[m]); }
  }

  SECTION("'abs()' on a positive quantity returns the abs")
  {
    SECTION("integral representation") { REQUIRE(abs(1 * isq::length[m]) == 1 * isq::length[m]); }

    SECTION("floating-point representation") { REQUIRE(abs(1. * isq::length[m]) == 1 * isq::length[m]); }
  }
}

TEST_CASE("numeric_limits functions", "[limits]")
{
  SECTION("'epsilon' works as expected using default floating type")
  {
    REQUIRE(epsilon<double>(isq::length[m]).number() ==
            std::numeric_limits<decltype(1. * isq::length[m])::rep>::epsilon());
  }
  SECTION("'epsilon' works as expected using integers")
  {
    REQUIRE(epsilon<int>(isq::length[m]).number() == std::numeric_limits<decltype(1 * isq::length[m])::rep>::epsilon());
  }
}

TEST_CASE("floor functions", "[floor]")
{
  SECTION("floor 1 second with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
  }
  SECTION("floor 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("floor 1001 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1001 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("floor 1999 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1999 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("floor -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("floor -999 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-999 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("floor 1.3 seconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1.3 * isq::time[s]) == 1 * isq::time[s]);
  }
  SECTION("floor -1.3 seconds with target unit second should be -2 seconds")
  {
    REQUIRE(floor<si::second>(-1.3 * isq::time[s]) == -2 * isq::time[s]);
  }
  SECTION("floor 1001. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1001. * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("floor 1999. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1999. * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("floor -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("floor -999. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-999. * isq::time[ms]) == -1 * isq::time[s]);
  }

  // TODO Add tests for `N`, `kN` and `kg * m / s2` i `kg * km / s2`
}

TEST_CASE("ceil functions", "[ceil]")
{
  SECTION("ceil 1 second with target unit second should be 1 second")
  {
    REQUIRE(ceil<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
  }
  SECTION("ceil 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(ceil<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("ceil 1001 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1001 * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("ceil 1999 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1999 * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("ceil -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("ceil -999 milliseconds with target unit second should be 0 seconds")
  {
    REQUIRE(ceil<si::second>(-999 * isq::time[ms]) == 0 * isq::time[s]);
  }
  SECTION("ceil 1.3 seconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1.3 * isq::time[s]) == 2 * isq::time[s]);
  }
  SECTION("ceil -1.3 seconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1.3 * isq::time[s]) == -1 * isq::time[s]);
  }
  SECTION("ceil 1001. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1001. * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("ceil 1999. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1999. * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("ceil -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("ceil -999. milliseconds with target unit second should be 0 seconds")
  {
    REQUIRE(ceil<si::second>(-999. * isq::time[ms]) == 0 * isq::time[s]);
  }
}

TEST_CASE("round functions", "[round]")
{
  SECTION("round 1 second with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
  }
  SECTION("round 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1001 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1001 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1499 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1499 * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1500 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1500 * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("round 1999 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1999 * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("round -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1001 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1001 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1499 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1499 * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1500 milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1500 * isq::time[ms]) == -2 * isq::time[s]);
  }
  SECTION("round -1999 milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1999 * isq::time[ms]) == -2 * isq::time[s]);
  }
  SECTION("round 1000. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1000. * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1001. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1001. * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1499. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1499. * isq::time[ms]) == 1 * isq::time[s]);
  }
  SECTION("round 1500. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1500. * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("round 1999. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1999. * isq::time[ms]) == 2 * isq::time[s]);
  }
  SECTION("round -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1001. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1001. * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1499. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1499. * isq::time[ms]) == -1 * isq::time[s]);
  }
  SECTION("round -1500. milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1500. * isq::time[ms]) == -2 * isq::time[s]);
  }
  SECTION("round -1999. milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1999. * isq::time[ms]) == -2 * isq::time[s]);
  }
}

TEST_CASE("hypot functions", "[hypot]")
{
  SECTION("hypot should work on the same quantities")
  {
    REQUIRE(hypot(3. * isq::length[km], 4. * isq::length[km]) == 5. * isq::length[km]);
    REQUIRE(hypot(2. * isq::length[km], 3. * isq::length[km], 6. * isq::length[km]) == 7. * isq::length[km]);
  }
  SECTION("hypot should work with different units of the same dimension")
  {
    REQUIRE(hypot(3. * isq::length[km], 4000. * isq::length[m]) == 5. * isq::length[km]);
    REQUIRE(hypot(2. * isq::length[km], 3000. * isq::length[m], 6. * isq::length[km]) == 7. * isq::length[km]);
  }
}

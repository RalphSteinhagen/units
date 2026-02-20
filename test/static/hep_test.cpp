// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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

#include "test_tools.h"
#include <mp-units/systems/hep.h>

namespace {

using namespace mp_units;
using namespace mp_units::hep::unit_symbols;
using enum mp_units::quantity_character;

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return q.character == ch && (... && requires { q[units]; });
}

// space and time
static_assert(verify(hep::length, real_scalar, mm, cm));  // Gaudi: mm, ROOT: cm
static_assert(verify(hep::area, real_scalar, mm2));
static_assert(verify(hep::volume, real_scalar, mm3));
static_assert(verify(hep::angle, real_scalar, hep::radian, hep::degree));  // Gaudi: radian, ROOT: degree
static_assert(verify(hep::solid_angle, real_scalar, hep::steradian));
static_assert(verify(hep::duration, real_scalar, ns, s));  // Gaudi: ns, ROOT: s

// electric
static_assert(verify(hep::electric_charge, real_scalar, hep::eplus));
static_assert(verify(hep::electric_current, real_scalar, hep::ampere));
static_assert(verify(hep::electric_potential, real_scalar, hep::volt));
static_assert(verify(hep::electric_resistance, real_scalar, hep::ohm));
static_assert(verify(hep::electric_capacitance, real_scalar, hep::farad));

// magnetic
static_assert(verify(hep::magnetic_flux, real_scalar, hep::weber));
static_assert(verify(hep::magnetic_field, real_scalar, hep::tesla));
static_assert(verify(hep::inductance, real_scalar, hep::henry));

// energy, power, force, pressure
static_assert(verify(hep::energy, real_scalar, MeV, GeV));  // Gaudi: MeV, ROOT: GeV
static_assert(verify(hep::power, real_scalar, hep::watt));
static_assert(verify(hep::force, real_scalar, hep::newton));
static_assert(verify(hep::pressure, real_scalar, hep::pascal));

// mechanical
static_assert(verify(hep::mass, real_scalar, hep::gram));
static_assert(verify(hep::frequency, real_scalar, hep::hertz));

// thermodynamic
static_assert(verify(hep::temperature, real_scalar, hep::kelvin));
static_assert(verify(hep::amount_of_substance, real_scalar, hep::mole));

// radiometric
static_assert(verify(hep::activity, real_scalar, hep::becquerel));
static_assert(verify(hep::absorbed_dose, real_scalar, hep::gray));

// photometric
static_assert(verify(hep::luminous_intensity, real_scalar, hep::candela));
static_assert(verify(hep::luminous_flux, real_scalar, hep::lumen));
static_assert(verify(hep::illuminance, real_scalar, hep::lux));

}  // namespace

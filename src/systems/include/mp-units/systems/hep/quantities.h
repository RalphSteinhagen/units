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

// The HEP system of quantities is inspired by popular HEP projects:
// - https://gitlab.cern.ch/CLHEP/CLHEP/-/blob/develop/Units/Units/SystemOfUnits.h
// - https://gitlab.cern.ch/gaudi/Gaudi/-/blob/main/GaudiKernel/include/GaudiKernel/SystemOfUnits.h
// - https://gitlab.cern.ch/geant4/geant4/-/blob/master/source/externals/clhep/include/CLHEP/Units/SystemOfUnits.h
// - https://github.com/root-project/root/blob/master/geom/geom/inc/TGeoSystemOfUnits.h

#pragma once

#include <mp-units/bits/module_macros.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::hep {

// dimensions of base quantities
// clang-format off
inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_angle final : base_dimension<symbol_text{u8"α", "a"}> {} dim_angle;
inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_charge final : base_dimension<"Q"> {} dim_electric_charge;
inline constexpr struct dim_energy final : base_dimension<"E"> {} dim_energy;
inline constexpr struct dim_temperature final : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_temperature;
inline constexpr struct dim_amount_of_substance final : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity final : base_dimension<"I"> {} dim_luminous_intensity;
// clang-format on

// base quantities
QUANTITY_SPEC(length, dim_length);
QUANTITY_SPEC(angle, dim_angle);
QUANTITY_SPEC(solid_angle, pow<2>(angle));
QUANTITY_SPEC(duration, dim_time);
QUANTITY_SPEC(electric_charge, dim_electric_charge);
QUANTITY_SPEC(energy, dim_energy);
QUANTITY_SPEC(temperature, dim_temperature);
QUANTITY_SPEC(amount_of_substance, dim_amount_of_substance);
QUANTITY_SPEC(luminous_intensity, dim_luminous_intensity);

// derived quantites
// space and time
QUANTITY_SPEC(area, pow<2>(length));
QUANTITY_SPEC(volume, pow<3>(length));

// electric
QUANTITY_SPEC(electric_current, electric_charge / duration);
QUANTITY_SPEC(electric_potential, energy / electric_charge);
QUANTITY_SPEC(electric_resistance, energy* duration / pow<2>(electric_charge));
QUANTITY_SPEC(electric_capacitance, pow<2>(electric_charge) / energy);

// magnetic
QUANTITY_SPEC(magnetic_flux, duration* energy / electric_charge);
QUANTITY_SPEC(magnetic_field, duration* energy / electric_charge / pow<2>(length));
QUANTITY_SPEC(inductance, pow<2>(duration) * energy / pow<2>(electric_charge));

// energy, power, force, pressure
QUANTITY_SPEC(power, energy / duration);
QUANTITY_SPEC(force, energy / length);
QUANTITY_SPEC(pressure, energy / pow<3>(length));

// mechanical
QUANTITY_SPEC(mass, energy* pow<2>(duration) / pow<2>(length));
QUANTITY_SPEC(frequency, inverse(duration));

// radiometric
QUANTITY_SPEC(activity, inverse(duration));
QUANTITY_SPEC(absorbed_dose, pow<2>(length) / pow<2>(duration));

// photometric
QUANTITY_SPEC(luminous_flux, luminous_intensity* solid_angle);
QUANTITY_SPEC(illuminance, luminous_flux / pow<2>(length));

}  // namespace mp_units::hep

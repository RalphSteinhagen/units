# High Energy Physics (HEP) System

## Overview

The **High Energy Physics (HEP)** system provides a complete **System of
Quantities** and **System of Units**, along with physical constants, tailored for
particle physics, nuclear physics, and high-energy experiments. It was designed
with input from CERN and the ATLAS experiment to address real-world requirements
of large-scale HEP software projects.

The **mp-units** HEP system implements:

- **System of Quantities**: A dimensional system with base quantities (_length_,
  _time_, _energy_, _electric charge_, _temperature_, _amount of substance_,
  _luminous intensity_, _angle_) that differs from the
  [International System of Quantities (ISQ)](isq.md)
- **System of Units**: Base units millimetre (mm), nanosecond (ns),
  megaelectronvolt (MeV), elementary charge (eplus), kelvin (K), mole (mol),
  candela (cd), radian (rad), steradian (sr)
- **Physical constants**: From CODATA 2014, 2018, and 2022 recommendations,
  allowing users to choose the appropriate version for their application
- **Interoperability**: Designed to work alongside existing HEP frameworks
  (CLHEP, Gaudi, Geant4, ROOT) during migration

!!! info "Collaboration with CERN"

    The HEP system design was developed through consultation with the ATLAS Offline Software
    team at CERN. The goal is to provide a modern, type-safe units library that can eventually
    replace hand-maintained unit constants across multiple HEP frameworks while maintaining
    compatibility during migration.

## System of Quantities

The HEP System of Quantities defines a dimensional system fundamentally different
from the [International System of Quantities (ISQ)](isq.md). While both systems describe
physical relationships between quantities, HEP makes different choices for base
quantities that are more natural for particle physics.

### Base Quantities and Dimensions

The HEP system uses eight base quantities:

| Base Quantity                   | Dimension Symbol |           Same as ISQ?            |
|---------------------------------|:----------------:|:---------------------------------:|
| **_length_**                    |        L         |              ✓ (yes)              |
| **_time_** (_duration_)         |        T         |              ✓ (yes)              |
| **_energy_**                    |        E         |       ✗ (ISQ uses _mass_ M)       |
| **_electric charge_**           |        Q         | ✗ (ISQ uses _electric current_ I) |
| **_thermodynamic temperature_** |        Θ         |              ✓ (yes)              |
| **_amount of substance_**       |        N         |              ✓ (yes)              |
| **_luminous intensity_**        |        I         |              ✓ (yes)              |
| **_angle_**                     |        α         |              ✗ (no)               |

### Key Differences from ISQ

**_Energy_ instead of _Mass_:**

In particle physics, _energy_ (E) is more fundamental than _mass_ (M) due to
_mass-energy_ equivalence ($E = mc²$). When physicists say "electron mass =
0.511 MeV", they're using natural units shorthand where c=1, but they're really
referring to rest energy. In the HEP system, _energy_ is the base quantity while
_mass_ is derived.

```cpp
using namespace mp_units::hep::unit_symbols;

// Access mass constants (dimension: M = E/L²T⁻²)
quantity electron_m = m_e;  // ≈ 0.511 MeV/c²
quantity proton_m = m_p;    // ≈ 938.3 MeV/c²

// Calculate rest energies from mass (dimension: E)
quantity electron_rest_energy = m_e * c2;  // E = mc² ≈ 0.511 MeV
quantity proton_rest_energy = m_p * c2;    // E = mc² ≈ 938.3 MeV

// In natural units where c=1, physicists often say "electron mass = 0.511 MeV"
// but in HEP system with explicit units, we distinguish mass from rest energy
```

**_Electric Charge_ instead of _Electric Current_:**

_Electric charge_ (Q) is treated as a base quantity rather than _electric current_.
This reflects the discrete nature of _charge_ in particle physics (multiples of
elementary _charge_) and makes _charge_ conservation explicit.

```cpp
// In HEP: electric charge is a base quantity
quantity proton_charge = 1. * eplus;
quantity electron_charge = -1. * eplus;
quantity up_quark_charge = 2. / 3. * eplus;

// Current is derived: I = Q/T
quantity current = proton_charge / (1. * ns);
```

### Derived Quantities

All other quantities are derived from these base quantities through dimensional
analysis:

- **_Force_**: F = E/L (_energy_ per _length_)
- **_Power_**: P = E/T (_energy_ per _time_)
- **_Electric Current_**: I = Q/T (_charge_ per _time_)
- **_Electric Potential_**: V = E/Q (_energy_ per _charge_)
- **_Magnetic Field_**: B = E·T/(Q·L²) (dimension: ETQ⁻¹L⁻²)
- **_Mass_**: M = E·T²/L² (derived from $E = Mc²$, where $c = L/T$)

This dimensional structure ensures dimensional consistency while using quantities
natural to particle physics calculations.

## System of Units (Base Units)

The HEP System of Units provides concrete measurement scales for the base
quantities. Unlike SI (which uses metre, second, kilogram, ampere), the HEP
system uses units natural to particle physics:

| Quantity              | Unit              | Symbol | Why?                                                                              |
|-----------------------|-------------------|:------:|-----------------------------------------------------------------------------------|
| **_Length_**          | millimetre        |   mm   | Detector geometries are typically millimetre-scale; avoids frequent powers of 10³ |
| **_Time_**            | nanosecond        |   ns   | Particle lifetimes and detector timing naturally in nanoseconds                   |
| **_Energy_**          | megaelectronvolt  |  MeV   | Particle energies range from MeV to TeV; MeV provides good numerical range        |
| **_Electric Charge_** | elementary charge | eplus  | Charge is quantized in multiples of e; makes charge quantum numbers explicit      |
| **_Temperature_**     | kelvin            |   K    | Same as SI (no HEP-specific convention)                                           |
| **_Amount_**          | mole              |  mol   | Same as SI (for material composition)                                             |
| **_Luminosity_**      | candela           |   cd   | Same as SI (for detector calibration)                                             |
| **_Angle_**           | radian            |  rad   | Same as SI (for angular measurements)                                             |

## Interoperability with Existing HEP Frameworks

The HEP system is designed to coexist with legacy unit definitions during
migration. This is crucial for large projects like ATLAS where multiple
frameworks maintain their own unit constants.

### Current Landscape

Major HEP frameworks currently use hand-maintained header files with `constexpr double` values:

=== "CLHEP"

    ```cpp
    // https://gitlab.cern.ch/CLHEP/CLHEP/-/blob/develop/Units/Units/SystemOfUnits.h
    namespace CLHEP {
      static constexpr double mm  = 1.;
      static constexpr double mm2 = mm*mm;
      static constexpr double mm3 = mm*mm*mm;
      
      static constexpr double centimeter = 10.*mm;
      static constexpr double meter = 1000.*mm;
      
      static constexpr double nanosecond = 1.;
      static constexpr double second = 1.e+9 *nanosecond;
      
      static constexpr double MeV = 1.;
      static constexpr double GeV = 1.e+3*MeV;
    }
    ```

=== "Gaudi"

    ```cpp
    // https://gitlab.cern.ch/gaudi/Gaudi/-/blob/main/GaudiKernel/include/GaudiKernel/SystemOfUnits.h
    namespace Gaudi {
      namespace Units {
        static constexpr double millimeter = 1.;
        static constexpr double millimeter2 = millimeter * millimeter;
        static constexpr double millimeter3 = millimeter * millimeter * millimeter;
        
        static constexpr double centimeter = 10. * millimeter;
        static constexpr double meter = 1000. * millimeter;
        
        static constexpr double nanosecond = 1.;
        static constexpr double second = 1.e+9 * nanosecond;
        
        static constexpr double MeV = 1.;
        static constexpr double GeV = 1.e+3 * MeV;
      }
    }
    ```

=== "Geant4"

    ```cpp
    // https://gitlab.cern.ch/geant4/geant4/-/blob/master/source/externals/clhep/include/CLHEP/Units/SystemOfUnits.h
    // Geant4 vendors CLHEP internally, uses same conventions
    namespace CLHEP {
      static constexpr double mm  = 1.;
      static constexpr double ns  = 1.;
      static constexpr double MeV = 1.;
      // ... etc
    }
    ```

=== "ROOT"

    ```cpp
    // https://github.com/root-project/root/blob/master/geom/geom/inc/TGeoSystemOfUnits.h
    namespace TGeoUnit {
      static constexpr double mm = 0.1;
      static constexpr double cm = 1.;     // Base unit (centimeter)
      static constexpr double m = 100.;
      
      // Note: ROOT uses seconds as base, differs from others!
      static constexpr double ns = 1.e-9;
      static constexpr double s = 1.;      // Base unit (second)
      
      static constexpr double GeV = 1.;    // Base unit (GeV not MeV!)
      static constexpr double MeV = 1.e-3;
    }
    ```

!!! warning "Inconsistencies Between Frameworks"

    Note that different frameworks choose different base units:
    
    - **_Length_**: CLHEP/Gaudi/Geant4 use mm=1, ROOT uses cm=1
    - **_Time_**: CLHEP/Gaudi/Geant4 use ns=1, ROOT uses s=1  
    - **_Energy_**: CLHEP/Gaudi/Geant4 use MeV=1, ROOT uses GeV=1
    
    These differences require careful attention at framework boundaries. The **mp-units** HEP 
    system uses mm, ns, and MeV as base units (matching the majority), but supports all 
    units with proper conversions.

### Migration Strategy

The recommended migration path is:

1. **Phase 1**: Create mp-units drop-in replacements for unit constant headers
2. **Phase 2**: Gradually introduce typed quantities in new code
3. **Phase 3**: Refactor existing code to use typed quantities where beneficial

```cpp
// Phase 1: Drop-in replacement (still using raw doubles)
namespace MyExperiment {
  // Instead of:
  // static constexpr double mm = 1.;
  // static constexpr double GeV = 1000.;
  
  // Use mp-units values cast to double for backward compatibility:
  static constexpr double mm = (1. * mp_units::hep::mm).numerical_value_in(mp_units::hep::mm);
  static constexpr double GeV = (1. * mp_units::hep::GeV).numerical_value_in(mp_units::hep::mm * mp_units::hep::energies);
}

// Phase 2: New code uses typed quantities
auto calculate_momentum(quantity<mp_units::hep::energy> E, 
                        quantity<mp_units::hep::mass> m) {
  using namespace mp_units::hep::unit_symbols;
  quantity p_sq = E * E - m * m * c2;
  return sqrt(p_sq);  // Returns quantity with momentum dimension
}

// Phase 3: Interface with legacy code
double legacy_function(double energy_in_GeV);  // Existing API

void modern_caller() {
  using namespace mp_units::hep::unit_symbols;
  quantity particle_E = 125. * GeV;
  
  // Convert to double for legacy interface
  double result = legacy_function(particle_E.numerical_value_in(GeV));
  
  // Convert result back to typed quantity
  quantity calculated_mass = result * GeV / c2;
}
```

See the
[Interoperability Guide](../../how_to_guides/integration/interoperability_with_other_libraries.md)
for more details on integrating mp-units with existing codebases.

## Physical Constants with CODATA Versions

One of the major challenges in HEP software is that physical constants change
over time as measurements improve. Different frameworks may use different CODATA
recommendations, leading to inconsistencies.

### The Problem

As noted by ATLAS Offline Software Coordinator John Chapman:

!!! quote "CERN Feedback"

    "The values used in the physical constants headers are taken from the CODATA recommendations. 
    Note that these do change over time! This is particularly a problem for applications where 
    it is then not clear which version of a constant should be used. It also, unfortunately, 
    ties any updates to physical constants to releases of CLHEP, ROOT, Gaudi or Geant4.
    
    For example, currently the head versions of CLHEP and Gaudi use the 2018 CODATA 
    recommendations and the head version of ROOT uses the 2022 CODATA recommendations.
    
    I would like to find a way to avoid this. If they all depended on a single external 
    library which we could easily configure to use a particular set of recommendations, then 
    that would make things easier."

### The Solution: CODATA Namespaces

The **mp-units** HEP system provides constants organized by CODATA release,
allowing explicit version selection:

```cpp
#include <mp-units/systems/hep.h>

using namespace mp_units;
using namespace mp_units::hep::unit_symbols;

// Default: CODATA 2018 (inline namespace - matches CLHEP, Gaudi, Geant4)
quantity kinetic_energy = 10. * MeV;
quantity beta_2018 = kinetic_energy / (hep::electron_mass * c2);  // Uses CODATA 2018

// Interface with ROOT analysis using CODATA 2022
quantity analyze_with_root()
{
  // All constants now use CODATA 2022 values to match ROOT
  quantity rest_energy = hep::codata2022::electron_mass * c2;  // 0.51099895069 MeV
  quantity momentum = sqrt(pow<2>(kinetic_energy + rest_energy) - pow<2>(rest_energy));
  return momentum;
}

// Reproduce legacy results with CODATA 2014
quantity validate_legacy_analysis(quantity<MeV> E_gamma)
{
  // Use older constants for exact reproducibility
  quantity threshold = 2. * hep::codata2014::electron_mass * c2;  // Pair production
  return (E_gamma > threshold) ? E_gamma - threshold : 0. * MeV;
}
```

### Key Differences Between CODATA Releases

| Constant                          | CODATA 2014        | CODATA 2018               | CODATA 2022               |
|-----------------------------------|--------------------|---------------------------|---------------------------|
| **Boltzmann constant (MeV/K)**    | 8.6173303×10⁻¹¹    | 8.617333262×10⁻¹¹ (exact) | 8.617333262×10⁻¹¹ (exact) |
| **Electron mass (MeV/c²)**        | 0.5109989461       | 0.51099895000             | 0.51099895069             |
| **Proton mass (MeV/c²)**          | 938.2720813        | 938.27208816              | 938.27208943              |
| **Neutron mass (MeV/c²)**         | 939.5654133        | 939.56542052              | 939.56542194              |
| **Fine structure constant**       | 7.2973525664×10⁻³  | 7.2973525693×10⁻³         | 7.2973525643×10⁻³         |
| **Classical electron radius (m)** | 2.8179403227×10⁻¹⁵ | 2.8179403262×10⁻¹⁵        | 2.8179403205×10⁻¹⁵        |

!!! note "SI 2019 Redefinition"

    Since the 2019 SI redefinition, several fundamental constants (Planck constant, Boltzmann 
    constant, Avogadro constant, elementary charge) are **exact by definition**. This is why 
    the Boltzmann constant doesn't change between CODATA 2018 and 2022 - it's now fixed.

### Available Constants

Constants that are exact (identical across all CODATA releases):

- `avogadro_constant` (Nₐ)
- `speed_of_light_in_vacuum` (c)
- `planck_constant` (h)
- `elementary_charge` (e)
- `permeability_of_vacuum` (μ₀)

Constants that vary by CODATA release (organized in `codata2014`,
`codata2018`, `codata2022` namespaces):

- `boltzmann_constant` (k_B) - exact since 2018
- `electron_mass` (mₑ)
- `proton_mass` (m_p)
- `neutron_mass` (m_n)
- `atomic_mass_unit` (u)
- `fine_structure_constant` (α)
- `classical_electron_radius` (rₑ)
- `electron_compton_wavelength` (λ_C)
- `bohr_radius` (a₀)
- `bohr_magneton` (μ_B)
- `nuclear_magneton` (μ_N)


## Use Cases from CERN ATLAS

The HEP system design incorporates requirements from real-world HEP experiments:

### Detector Geometry

```cpp
using namespace mp_units::hep::unit_symbols;

// Inner detector barrel radius
quantity barrel_radius = 371. * mm;

// Silicon strip sensor dimensions
quantity sensor_length = 128. * mm;
quantity sensor_width = 12.8 * cm;
quantity sensor_area = sensor_length * sensor_width;

// Material thickness (radiation lengths)
quantity silicon_thickness = 300. * um;  // micrometres
```

### Particle Tracking

```cpp
using namespace mp_units::hep::unit_symbols;

// Track momentum in transverse plane
quantity p_T = 45. * GeV;

// Pseudorapidity (dimensionless, but type-safe)
quantity eta = 2.1 * one;

// Azimuthal angle
quantity phi = 1.57 * rad;

// Invariant mass calculation
quantity invariant_mass(quantity<GeV> E1, quantity<GeV> E2,
                        quantity<GeV> p1, quantity<GeV> p2,
                        quantity<rad> delta_phi,
                        quantity<one> delta_eta)
{
  quantity p1_sq = pow<2>(p1);
  quantity p2_sq = pow<2>(p2);
  quantity m1_sq = pow<2>(E1) - p1_sq * c2;
  quantity m2_sq = pow<2>(E2) - p2_sq * c2;
  
  quantity m_inv_sq = m1_sq + m2_sq + 2. * (E1 * E2 - p1 * p2 * c2 * (cosh(delta_eta) - cos(delta_phi)));
  return sqrt(m_inv_sq);
}
```

### Particle Lifetimes and Decay

```cpp
using namespace mp_units::hep::unit_symbols;

// Muon lifetime
quantity tau_muon = 2197. * ns;

// Proper time calculation
quantity<ns> proper_time(quantity<ns> lab_time,
                         quantity<one> gamma) {
  return lab_time / gamma;
}

// Lorentz factor
auto lorentz_factor(quantity<GeV> p, 
                    quantity<GeV> m) {
  using namespace mp_units::hep::unit_symbols;
  auto E = sqrt(pow<2>(p) * c2 + pow<2>(m) * pow<2, 2>(c));
  return E / (m * c2);
}
```

### Calorimeter Energy Deposits

```cpp
// Electromagnetic calorimeter
quantity em_energy = 125.5 * GeV;  // Higgs candidate

// Hadronic calorimeter
quantity had_energy = 450. * GeV;  // Jet energy

// Missing transverse energy (neutrinos, dark matter)
quantity missing_E_T = 85. * GeV;
```

### Cross Sections

```cpp
// Particle physics cross sections (natural unit: GeV⁻²)
quantity higgs_xsec = 48.6 * pb;  // picobarns

// Convert to alternative units
auto xsec_in_mb = higgs_xsec.in(mb);  // millibarns
auto xsec_in_fm2 = higgs_xsec.in(fm * fm);  // square femtometres
```

### Event Timing

```cpp
// Bunch crossing intervals at LHC
quantity bunch_spacing = 25. * ns;

// Time-of-flight measurement
quantity flight_distance = 2400. * mm;
auto time_of_flight = flight_distance / c;

// Detector timing resolution
quantity timing_resolution = 50. * ps;  // picoseconds
```

## API Reference

For complete API documentation including all available units, constants, and examples:

- [HEP System Reference](../../reference/systems_reference/systems/hep.md) -
  Complete reference for all dimensions, quantities, units, and prefixes

## Further Reading

- **CODATA Recommendations**:
    - [CODATA 2014](https://physics.nist.gov/cuu/Constants/archive2014.html)
    - [CODATA 2018](https://physics.nist.gov/cuu/Constants/archive2018.html)
    - [CODATA 2022](https://physics.nist.gov/cuu/Constants/) (current)

- **Particle Data Group**: [pdg.lbl.gov](https://pdg.lbl.gov) - Particle
  properties and fundamental constants used in Monte Carlo event generators

- **CERN Software**:
    - [CLHEP](https://gitlab.cern.ch/CLHEP/CLHEP) - Class Library for High Energy Physics
    - [Gaudi](https://gitlab.cern.ch/gaudi/Gaudi) - Event processing framework
    - [Geant4](https://gitlab.cern.ch/geant4/geant4) - Detector simulation toolkit
    - [ROOT](https://github.com/root-project/root) - Data analysis framework

- **ATLAS Experiment**: [atlas.cern](https://atlas.cern) - Large Hadron
  Collider experiment at CERN

- **HEP Software Foundation**:
  [hepsoftwarefoundation.org](https://hepsoftwarefoundation.org/) -
  Coordination of software development across HEP experiments

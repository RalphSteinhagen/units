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
quantities that are more natural for particle physics. The two systems are
**mutually incompatible by design** — there is no implicit conversion between them.

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

Two base quantities differ from ISQ, making the two systems dimensionally
incompatible — the compiler rejects any attempt to mix quantities from both:

```cpp
quantity<isq::length[si::metre]>  L_isq = 1. * si::metre;
quantity<hep::length[hep::metre]> L_hep = 1. * hep::metre;

// auto bad = L_isq + L_hep;  // Compile error — different dimensional systems ✓
```

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

## Specialized Quantities of the Same Kind

Beyond basic dimensional safety, the HEP system provides **specialized quantities**
to distinguish physically distinct concepts that share the same dimension. This
addresses real problems in particle physics code where mixing conceptually different
values is physically meaningless but dimensionally valid.

The HEP system provides specialized quantities for:

- **Energy**: `total_energy`, `kinetic_energy`, `rest_mass_energy`, `binding_energy`,
  `excitation_energy`, `ionization_energy`, `threshold_energy`, `Q_value`, and more
- **Length**: `path_length`, `displacement`, `decay_length`, `radiation_length`,
  `interaction_length`, `impact_parameter`, `wavelength`, `range`, and more
- **Time**: `proper_time`, `coordinate_time`, `mean_lifetime`, `half_life`,
  `time_of_flight`
- **Mass**: `rest_mass`, `invariant_mass`, `effective_mass`, `reduced_mass`
- **Momentum**: `momentum`, `transverse_momentum`
- **Angle**: `scattering_angle`, `opening_angle`, `azimuthal_angle`, `polar_angle`
- **Dimensionless (special)**: `lorentz_factor` (γ = E/E₀), `relativistic_beta` (β = v/c)
- **Interaction**: `cross_section` (σ), `number_density` (n), `decay_constant` (λ)

For complete hierarchy trees and all available specialized quantities, see the
[HEP System Reference](../../reference/systems_reference/systems/hep.md).

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

### The "Multiply to Set, Divide to Get" Pattern

Understanding the legacy unit pattern is essential for migration. In CLHEP/Geant4/Gaudi/ROOT,
units are simply `constexpr double` scaling factors:

```cpp
namespace CLHEP {
  static constexpr double mm = 1.;        // Base unit
  static constexpr double cm = 10. * mm;  // = 10.0
  static constexpr double MeV = 1.;       // Base unit  
  static constexpr double GeV = 1.e+3 * MeV;  // = 1000.0
}
```

The pattern is:

- **Multiply to create values**: `double energy = 50.0 * GeV;` stores `50000.0`
  (in base units: MeV)
- **Divide to extract/convert**: `double val_in_gev = energy / GeV;` returns `50.0`
- **APIs expect bare doubles**: Function parameters are `double`, documented to expect
  specific units

```cpp
// Creating values - multiply by unit constant
double particle_energy = 50.0 * GeV;   // Stores 50000.0 MeV internally
double track_length = 2.5 * meter;     // Stores 2500.0 mm internally

// Converting - divide by unit constant  
std::cout << "Energy in MeV: " << particle_energy / MeV << "\n";   // Prints 50000
std::cout << "Length in cm: " << track_length / cm << "\n";        // Prints 250

// API consumption - pass bare double (assumed to be in base units)
void processParticle(double energy_MeV, double length_mm);  // Units in documentation only!
processParticle(particle_energy, track_length);  // Passes 50000.0, 2500.0
```

**The fundamental problems**:

1. **No dimensional safety**: `double result = energy + length;` compiles without error
2. **API ambiguity**: `setRadius(double r)` - is `r` in mm? cm? Must check documentation
3. **Conversion confusion**: When to multiply vs divide? Easy to get backwards
4. **Silent logic bugs**: Forgetting to multiply/divide gives wrong numerical values with
   no warning

This is why **mp-units** is valuable: it makes units and dimensions explicit in the type system.

### Migration Strategy

#### Phase 1: Drop-in replacements for unit constant headers

Replace hand-maintained `constexpr double` unit constants with mp-units-derived
values. All existing code that multiplies and divides by these constants continues
to compile and produce identical results — no other changes needed at this stage.

=== "Before"

    ```cpp
    // Hand-maintained constants — values must be kept in sync manually
    namespace CLHEP {
      static constexpr double mm  = 1.;             // Base unit
      static constexpr double cm  = 10. * mm;       // = 10.0
      static constexpr double MeV = 1.;             // Base unit
      static constexpr double GeV = 1.e+3 * MeV;   // = 1000.0
    }
    ```

=== "After"

    ```cpp
    // mp-units derives the values — no manual arithmetic, always correct
    namespace CLHEP {
      using namespace mp_units::hep::unit_symbols;
      static constexpr double mm  = (1. * mm ).numerical_value_in(mm );  // = 1.0  (base unit)
      static constexpr double cm  = (1. * cm ).numerical_value_in(mm );  // = 10.0
      static constexpr double MeV = (1. * MeV).numerical_value_in(MeV);  // = 1.0  (base unit)
      static constexpr double GeV = (1. * GeV).numerical_value_in(MeV);  // = 1000.0
    }
    ```

#### Phase 2: New code uses typed quantities from the start

All newly written functions and classes use `quantity<...>` types directly. Legacy
code is untouched; new callsites enjoy full dimensional safety.

```cpp
quantity<hep::momentum[GeV / c]>
calculate_momentum(quantity<hep::energy[GeV]> energy, quantity<hep::mass[GeV / c2]> mass)
{
  return sqrt(pow<2>(energy) - pow<2>(mass * c2)) / c;
}
```

#### Phase 3: Wrap existing interfaces with type-safe overloads

Add typed setter overloads to existing classes. The class internals (data members,
private implementation) are left completely unchanged — they still store raw
`double`s. The new typed overloads simply convert at the public boundary and delegate
to the existing `double`-based methods. New callers use the typed API; old callers with
raw `double`s still compile without any modification.

!!! note

    The typed **getter** overloads cannot be added at this stage: C++ does not allow
    overloading on return type alone, so the existing `double` getters must remain
    unchanged. Only setters — which differ by parameter type — can be overloaded.

#### Phase 4: Refactor internals; add typed getters; deprecate `double` API

Once all external interfaces are wrapped and there is time to tackle the internals,
refactor the class to store typed quantities as data members. Add new typed getters
with a `_qty` suffix (e.g., `getMass_qty()`) alongside the existing `double` getters.
Deprecate both the old `double` setters and getters, driving migration to the typed API.

!!! note

    Because C++ cannot overload on return type alone, typed getters must use different
    names. The `_qty` suffix makes scripted migration easy: consumers can mechanically
    replace `getMass_qty()` → `getMass()` in Phase 5. Old code continues to work with
    deprecation warnings; new code uses the typed interface.

#### Phase 5: Complete the migration; remove `_qty` suffix

Once all call sites have migrated to the typed API (no more uses of deprecated `double`
methods), remove the deprecated overloads and rename the typed getters to their final
names. Since all consumers are already using typed quantities, this phase is just a
simple rename with no logic changes.

!!! tip

    This phase can be done with a simple regex replace: `s/(\w+)_qty\(\)/\1()/g`.
    For example, `getMass_qty()` → `getMass()`. All the heavy lifting (migrating to typed
    quantities) was already done in Phase 4 when consumers switched away from the deprecated API.

=== "Phase 3"

    ```cpp
    class Particle {
      double m_mass{};    // Still raw double — internals untouched
      double m_width{};
      double m_charge{};
    public:
      // Existing double-based interface — unchanged, still works:
      double getMass()   const { return m_mass; }
      double getWidth()  const { return m_width; }
      double getCharge() const { return m_charge; }

      void setMass(double mass_MeV_per_c2)  { m_mass  = mass_MeV_per_c2; }
      void setWidth(double width_MeV)       { m_width = width_MeV; }
      void setCharge(double charge_eplus)   { m_charge = charge_eplus; }

      // New typed overloads — convert at the boundary, delegate to existing setters:
      void setMass(quantity<hep::rest_mass[MeV / c2]> mass)
      {
        setMass(mass.numerical_value_in(MeV / c2));
      }
      void setWidth(quantity<hep::decay_width[MeV]> width)
      {
        setWidth(width.numerical_value_in(MeV));
      }
      void setCharge(quantity<hep::electric_charge[eplus]> charge)
      {
        setCharge(charge.numerical_value_in(eplus));
      }
    };
    ```

=== "Phase 4"

    ```cpp
    class Particle {
      quantity<hep::rest_mass[MeV / c2]>    m_mass{};    // Now typed quantities
      quantity<hep::decay_width[MeV]>       m_width{};
      quantity<hep::electric_charge[eplus]> m_charge{};
    public:
      // New typed interface — getters with _qty suffix and setters:
      quantity<hep::rest_mass[MeV / c2]>    getMass_qty()   const { return m_mass; }
      quantity<hep::decay_width[MeV]>       getWidth_qty()  const { return m_width; }
      quantity<hep::electric_charge[eplus]> getCharge_qty() const { return m_charge; }

      void setMass(quantity<hep::rest_mass[MeV / c2]> mass)         { m_mass  = mass; }
      void setWidth(quantity<hep::decay_width[MeV]> width)          { m_width = width; }
      void setCharge(quantity<hep::electric_charge[eplus]> charge)  { m_charge = charge; }

      // Old double interface — now deprecated:
      [[deprecated("Use getMass_qty() instead")]]
      double getMass()   const { return m_mass.numerical_value_in(MeV / c2); }
      [[deprecated("Use getWidth_qty() instead")]]
      double getWidth()  const { return m_width.numerical_value_in(MeV); }
      [[deprecated("Use getCharge_qty() instead")]]
      double getCharge() const { return m_charge.numerical_value_in(eplus); }

      [[deprecated("Use setMass(quantity<hep::rest_mass[MeV / c2]>) instead")]]
      void setMass(double mass_MeV_per_c2)
      {
        setMass(mass_MeV_per_c2 * MeV / c2);
      }
      [[deprecated("Use setWidth(quantity<hep::decay_width[MeV]>) instead")]]
      void setWidth(double width_MeV)
      {
        setWidth(width_MeV * MeV);
      }
      [[deprecated("Use setCharge(quantity<hep::electric_charge[eplus]>) instead")]]
      void setCharge(double charge_eplus)
      {
        setCharge(charge_eplus * eplus);
      }
    };
    ```

=== "Phase 5"

    ```cpp
    class Particle {
      quantity<hep::rest_mass[MeV / c2]>    m_mass{};
      quantity<hep::decay_width[MeV]>       m_width{};
      quantity<hep::electric_charge[eplus]> m_charge{};
    public:
      // Fully typed interface — deprecated methods removed, _qty suffix removed:
      quantity<hep::rest_mass[MeV / c2]>    getMass()   const { return m_mass; }
      quantity<hep::decay_width[MeV]>       getWidth()  const { return m_width; }
      quantity<hep::electric_charge[eplus]> getCharge() const { return m_charge; }

      void setMass(quantity<hep::rest_mass[MeV / c2]> mass)         { m_mass  = mass; }
      void setWidth(quantity<hep::decay_width[MeV]> width)          { m_width = width; }
      void setCharge(quantity<hep::electric_charge[eplus]> charge)  { m_charge = charge; }
    };
    ```


## Type Safety in Practice

The following examples demonstrate four categories of bugs that specialized quantity
types eliminate: dimensional errors, argument swaps, unit scale mismatches, and
same-kind confusion. Each example compares legacy `double`-based code with the
type-safe mp-units equivalent.

For all examples, consider this typical HEP API:

=== "Legacy API"

    ```cpp
    // All parameters are just double - units documented in comments
    void setParticleProperties(double mass_MeV_per_c2,   // Mass in MeV/c²
                               double width_MeV,         // Decay width in MeV
                               double charge);           // Electric charge
    ```

=== "Type-safe API"

    ```cpp
    // Types enforce correct dimensions and units at compile time
    void setParticleProperties(quantity<hep::rest_mass[MeV / c2]> mass,
                               quantity<hep::decay_width[MeV]> width,
                               quantity<hep::electric_charge[eplus]> charge);
    ```

### Wrong Dimensions

Passing a value with incorrect physical dimensions is a common error. For example,
passing _energy_ (E) where _mass_ (E/c²) is expected. In legacy code, the compiler
cannot detect this — both are just `double`. The wrong numerical value flows through
calculations, producing physically incorrect results.

=== "Legacy (unsafe)"

    ```cpp
    double higgs_mass = 125.0 * GeV;  // Energy in MeV (125'000.0), not mass!
    double higgs_width = 4.0 * MeV;

    // Should have divided by c_squared first - but compiler doesn't know!
    setParticleProperties(higgs_mass, higgs_width, 0.0);
    // Passes 125'000.0 as mass_MeV_per_c2 - catastrophically wrong
    // ✓ Compiles  ☠️ Wrong physics  ❌ No warning
    ```

=== "mp-units (safe)"

    ```cpp
    quantity higgs_mass = 125.0 * GeV;  // Compiler knows this is energy
    quantity higgs_width = 4.0 * MeV;

    // setParticleProperties(higgs_mass, higgs_width, 0.0 * eplus);
    // Compile error! Cannot convert energy to mass
    // "note: no known conversion from energy to rest_mass"

    // Must explicitly convert - the dimension error is caught:
    setParticleProperties(higgs_mass / c2, higgs_width, 0.0 * eplus);  // ✓
    ```

### Arguments Swapped

Function parameters with the same underlying type (`double`) but different semantic
meanings can be accidentally swapped. The compiler has no way to detect this in
legacy code because all parameters have identical types.

=== "Legacy (unsafe)"

    ```cpp
    double higgs_mass = 125.0 * GeV / c_squared;
    double higgs_width = 4.0 * MeV;

    // Accidentally reversed the arguments!
    setParticleProperties(higgs_width,  // should be higgs_mass
                          higgs_mass,   // should be higgs_width
                          0.0);
    // Results: mass=4.0 MeV/c² (off by 31'000×), width=125'000 MeV
    // ✓ Compiles  ☠️ Catastrophic  ❌ No warning
    ```

=== "mp-units (safe)"

    ```cpp
    quantity higgs_mass = 125.0 * GeV / c2;
    quantity higgs_width = 4.0 * MeV;

    // setParticleProperties(higgs_width, higgs_mass, 0.0 * eplus);
    // Compile error! Types don't match
    // "note: no known conversion from decay_width to rest_mass"

    setParticleProperties(higgs_mass, higgs_width, 0.0 * eplus);  // ✓
    ```

### Wrong Unit Scale (Framework Mismatch)

Different HEP frameworks use different base units (ROOT uses GeV=1, CLHEP uses MeV=1).
When interfacing between frameworks, forgetting to convert units produces values
off by factors of 1000 or more. In legacy code, these are just `double` values —
the compiler cannot detect the mismatch.

=== "Legacy (unsafe)"

    ```cpp
    double higgs_mass = 125.0 * GeV / c_squared;
    double higgs_width = get_from_ROOT();  // Returns 0.004 (ROOT uses GeV=1)

    // Forgot to convert ROOT's GeV to CLHEP's MeV!
    setParticleProperties(higgs_mass, higgs_width, 0.0);
    // API expects MeV, receives 0.004 → width = 0.004 MeV (off by 1000×!)
    // ✓ Compiles  ☠️ 1000× wrong  ❌ No warning
    ```

=== "mp-units (safe)"

    ```cpp
    quantity higgs_mass = 125.0 * GeV / c2;
    double higgs_width = get_from_ROOT();  // Returns bare 0.004

    // setParticleProperties(higgs_mass, higgs_width, 0.0 * eplus);
    // Compile error! Cannot convert double to quantity
    // Forces explicit unit attachment

    // Must specify the unit - conversion happens automatically:
    setParticleProperties(higgs_mass, higgs_width * GeV, 0.0 * eplus);
    // ✓ Compiles! 0.004 * GeV = 4 MeV (automatic conversion)
    ```

### Wrong Quantity of the Same Kind

Some physically distinct concepts share the same dimension but have different meanings.
For example, _invariant mass_ reconstructed from decay products vs. _rest mass_ of a
particle species. Mixing these is physically meaningless even though they're
dimensionally identical. Legacy `double` code cannot distinguish them.

=== "Legacy (unsafe)"

    ```cpp
    // From 4-momenta reconstruction
    double compute_invariant_mass();

    double m_inv = compute_invariant_mass();
    double higgs_width = 4.0 * MeV;

    // Invariant mass of ONE EVENT ≠ rest mass of THE HIGGS BOSON
    setParticleProperties(m_inv, higgs_width, 0.0);
    // Both are mass dimension (MeV/c²) - compiler cannot tell them apart
    // ✓ Compiles  ☠️ Wrong physics  ❌ No warning
    ```

=== "mp-units (safe)"

    ```cpp
    // From 4-momenta reconstruction
    quantity<hep::invariant_mass[GeV / c2]> compute_invariant_mass();

    quantity m_inv = compute_invariant_mass();
    quantity higgs_width = hep::decay_width(4.0 * MeV);

    // setParticleProperties(m_inv, higgs_width, 0.0 * eplus);
    // Compile error! Wrong quantity kind
    // "note: no known conversion from invariant_mass to rest_mass"
    // Physics constraints enforced by type system
    ```

### Summary

The mp-units type system makes all four classes of error **impossible** — they're
caught at compile time before the code ever runs. The physics constraints are
enforced by the compiler, not just by documentation and code review.

The type hierarchy also captures physical relationships. For _energy_, adding child
quantities yields their common parent, enforcing E = KE + mc²:

```cpp
using namespace mp_units::hep::unit_symbols;

quantity KE = hep::kinetic_energy(100. * GeV);
quantity E0 = hep::rest_mass_energy(938.27 * MeV);  // proton rest energy

// Addition yields the common parent: total_energy
quantity E_total = KE + E0;  // quantity<hep::total_energy[...]>

// The result cannot be silently narrowed back to a child type:
// quantity<hep::kinetic_energy[MeV]> KE2 = E_total;    // Compile error!
// quantity<hep::rest_mass_energy[MeV]> E02 = E_total;  // Compile error!
```

For other quantity categories (_lengths_, _times_, _masses_, _angles_), specialized types
are siblings — they prevent implicit mixing but share a common parent for explicit
conversions.

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

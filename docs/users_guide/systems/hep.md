# High Energy Physics (HEP) System

## Overview

The **High Energy Physics (HEP)** system provides a complete **System of
Quantities** and **System of Units**, along with physical constants, tailored for
particle physics, nuclear physics, and high-energy experiments. It was designed
with input from members of the high energy physics community to address real-world
requirements of large-scale HEP software projects.

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

!!! info "Collaboration with the HEP community"

    The HEP system design was developed through consultation with members of the HEP community.
    The goal is to provide a modern, type-safe units library that can eventually replace
    hand-maintained unit constants across multiple HEP frameworks while maintaining compatibility
    during migration.


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

Beyond the basic dimensional safety, the HEP system provides **specialized quantities**
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

## Distinct Quantity Subkinds

Some HEP quantities share the same dimension, unit,  and even the same parent quantity,
yet represent fundamentally **incompatible physical concepts** that should never be added
to or compared with each other. The HEP system uses the `is_kind` specifier to enforce
this at compile time. See
[Creating Distinct Quantity Kinds with `is_kind`](../framework_basics/systems_of_quantities.md#creating-distinct-quantity-kinds-with-is_kind)
for the general mechanism.

### Relativistic Time: `proper_time` and `coordinate_time`

Both are sub-quantities of `duration`, but they live in different reference frames and
are related by the Lorentz factor, not by addition:

$$\tau = \frac{t}{\gamma}$$

Adding a proper time to a coordinate time is a classic special-relativity mistake —
one is Lorentz-invariant, the other is frame-dependent:

```cpp
quantity tau = 10. * hep::ns * proper_time;  // particle rest frame
quantity t   = 25. * hep::ns * coordinate_time;  // lab frame

// auto wrong = tau + t;  // Compile error — different kinds ✓
```

To convert between them, the Lorentz factor must be applied explicitly:

```cpp
quantity gamma = 2.5 * lorentz_factor;
quantity t_lab = coordinate_time(tau * gamma);  // explicit physics conversion
```

### Material Characteristic Lengths: `radiation_length` and `interaction_length`

These are properties of a material, not distances travelled by a particle. They appear
as **denominators** in physics expressions (number of radiation lengths traversed = path
/ X₀), never as addends.

- **`radiation_length` (X₀)**: mean distance over which an electron loses 1/e of its
  energy through Bremsstrahlung — an electromagnetic material property.
- **`interaction_length` (λ_I)**: mean free path before a hadronic nuclear interaction —
  a nuclear material property.
- **`nuclear_interaction_length`**: a specialization of `interaction_length` for nuclear
  (as opposed to hadronic-elastic) interactions.

All three are distinct from each other and from geometric lengths:

```cpp
quantity X0      = 88.97 * hep::mm * radiation_length;      // lead X₀
quantity lambda_I = 194.4 * hep::mm * interaction_length;   // lead λ_I
quantity path    = 50. * hep::mm * path_length;

// auto wrong1 = X0 + lambda_I;  // Compile error — different kinds ✓
// auto wrong2 = X0 + path;      // Compile error — different kinds ✓

// Correct usage: use as a denominator
quantity n_X0 = path / X0;                             // dimensionless: number of X₀ traversed
```

### Dimensionless Relativistic Quantities

`lorentz_factor` (γ) and `relativistic_beta` (β) are dimensionless quantities with
specific physical meaning that must not be mixed with generic dimensionless values or
with each other:

- **`lorentz_factor` (γ = E/E₀)**: characterises relativistic time dilation and length
  contraction; ranges from 1 (at rest) to ∞.
- **`relativistic_beta` (β = v/c)**: velocity expressed as a fraction of the speed of
  light; ranges from 0 to 1. Related to γ by $\beta = \sqrt{1 - 1/\gamma^2}$.
- **`phase`**: quantum mechanical phase angle — a cyclic, dimensionless quantity
  incompatible with both γ and β and with the angular `angle` kind.

```cpp
quantity gamma  = 10. * lorentz_factor;
quantity beta   = 0.995 * relativistic_beta;
quantity phi    = 1.57 * phase;

// auto wrong1 = gamma + beta;   // Compile error — different kinds ✓
// auto wrong2 = gamma + phi;    // Compile error — different kinds ✓
// auto wrong3 = gamma + 1.;     // Compile error — not a plain dimensionless ✓

// Physics: recover beta from gamma
quantity beta_from_gamma = relativistic_beta(sqrt(1. - 1. / pow<2>(gamma / lorentz_factor)));
```


## Physical Constants with CODATA Versions

One of the major challenges in HEP software is that physical constants change
over time as measurements improve. Different frameworks may use different CODATA
recommendations, leading to inconsistencies.

### The Problem

As noted by John Chapman (currently one of the ATLAS Offline Software coordinators):

!!! quote "HEP community feedback"

    "HEP Software stacks typically rely on multiple projects. Each of which define their
    physical constants based on the CODATA recommendations. (Note these do change over time!)

    Currently the CODATA version is fixed for a particular release of a project, which can
    lead to (small) inconsistencies, for example if an older version of one project needs to
    be used, but another project needs to be updated to pick up new functionality.

    I would like to find a way to avoid this. If they all depended on a single external
    library which we could easily configure to use a particular set of recommendations,
    then that would make things easier."

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

- **[CERN Software](https://ep-dep-sft.web.cern.ch)**:
    - [CLHEP](https://lcginfo.cern.ch/pkg/clhep) [[git](https://gitlab.cern.ch/CLHEP/CLHEP)] - Class Library for High Energy Physics
    - [Gaudi](https://lcginfo.cern.ch/pkg/Gaudi) [[git](https://gitlab.cern.ch/gaudi/Gaudi)] - Event processing framework
    - [Geant4](https://lcginfo.cern.ch/pkg/Geant4) [[git](https://gitlab.cern.ch/geant4/geant4)] - Detector simulation toolkit
    - [ROOT](https://lcginfo.cern.ch/pkg/ROOT) [[git](https://github.com/root-project/root)] - Data analysis framework

- **ATLAS Experiment**: [atlas.cern](https://atlas.cern) - Large Hadron Collider experiment
  at CERN

- **HEP Software Foundation**:
  [hepsoftwarefoundation.org](https://hepsoftwarefoundation.org/) -
  Coordination of software development across HEP experiments

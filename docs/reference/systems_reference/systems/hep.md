<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# HEP System

**Namespace:** `mp_units::hep`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/hep.h>`

**Secondary Headers:**

- `<mp-units/systems/hep/quantities.h>`
- `<mp-units/systems/hep/units.h>`

## Dimensions

| Name | Symbol |
|------|:------:|
| <span id="dim_amount_of_substance"></span>`dim_amount_of_substance` | N |
| <span id="dim_angle"></span>`dim_angle` | α |
| <span id="dim_electric_charge"></span>`dim_electric_charge` | Q |
| <span id="dim_energy"></span>`dim_energy` | E |
| <span id="dim_length"></span>`dim_length` | L |
| <span id="dim_luminous_intensity"></span>`dim_luminous_intensity` | I |
| <span id="dim_temperature"></span>`dim_temperature` | Θ |
| <span id="dim_time"></span>`dim_time` | T |

## Quantities

| Quantity | Character | Dimension | is_kind | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:-------:|:------:|----------|:---------:|
| <span id="absorbed_dose"></span><code>absorbed_<wbr>dose</code> | Real | L²T⁻² | ✓ | <code>[absorbed_<wbr>dose](#absorbed_dose)</code> | — | <code>pow<2>([length](#length)) / <wbr>pow<2>([duration](#duration))</code> | [view](../hierarchies/absorbed_dose_hep.md) |
| <span id="activity"></span><code>activity</code> | Real | T⁻¹ | ✓ | <code>[activity](#activity)</code> | — | <code>inverse([duration](#duration))</code> | [view](../hierarchies/activity_hep.md) |
| <span id="amount_of_substance"></span><code>amount_<wbr>of_<wbr>substance</code> | Real | N | ✓ | <code>[amount_<wbr>of_<wbr>substance](#amount_of_substance)</code> | — | — | [view](../hierarchies/amount_of_substance_hep.md) |
| <span id="angle"></span><code>angle</code> | Real | α | ✓ | <code>[angle](#angle)</code> | — | — | [view](../hierarchies/angle_hep.md) |
| <span id="area"></span><code>area</code> | Real | L² | ✓ | <code>[area](#area)</code> | — | <code>pow<2>([length](#length))</code> | [view](../hierarchies/area_hep.md) |
| <span id="duration"></span><code>duration</code> | Real | T | ✓ | <code>[duration](#duration)</code> | — | — | [view](../hierarchies/duration_hep.md) |
| <span id="electric_capacitance"></span><code>electric_<wbr>capacitance</code> | Real | Q²E⁻¹ | ✓ | <code>[electric_<wbr>capacitance](#electric_capacitance)</code> | — | <code>pow<2>([electric_<wbr>charge](#electric_charge)) / <wbr>[energy](#energy)</code> | [view](../hierarchies/electric_capacitance.md) |
| <span id="electric_charge"></span><code>electric_<wbr>charge</code> | Real | Q | ✓ | <code>[electric_<wbr>charge](#electric_charge)</code> | — | — | [view](../hierarchies/electric_charge_hep.md) |
| <span id="electric_current"></span><code>electric_<wbr>current</code> | Real | QT⁻¹ | ✓ | <code>[electric_<wbr>current](#electric_current)</code> | — | <code>[electric_<wbr>charge](#electric_charge) / <wbr>[duration](#duration)</code> | [view](../hierarchies/electric_current_hep.md) |
| <span id="electric_potential"></span><code>electric_<wbr>potential</code> | Real | EQ⁻¹ | ✓ | <code>[electric_<wbr>potential](#electric_potential)</code> | — | <code>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge)</code> | [view](../hierarchies/electric_potential_hep.md) |
| <span id="electric_resistance"></span><code>electric_<wbr>resistance</code> | Real | ETQ⁻² | ✓ | <code>[electric_<wbr>resistance](#electric_resistance)</code> | — | <code>[energy](#energy) * <wbr>[duration](#duration) / <wbr>pow<2>([electric_<wbr>charge](#electric_charge))</code> | [view](../hierarchies/electric_resistance.md) |
| <span id="energy"></span><code>energy</code> | Real | E | ✓ | <code>[energy](#energy)</code> | — | — | [view](../hierarchies/energy_hep.md) |
| <span id="force"></span><code>force</code> | Real | EL⁻¹ | ✓ | <code>[force](#force)</code> | — | <code>[energy](#energy) / <wbr>[length](#length)</code> | [view](../hierarchies/force_hep.md) |
| <span id="frequency"></span><code>frequency</code> | Real | T⁻¹ | ✓ | <code>[frequency](#frequency)</code> | — | <code>inverse([duration](#duration))</code> | [view](../hierarchies/frequency_hep.md) |
| <span id="illuminance"></span><code>illuminance</code> | Real | α²IL⁻² | ✓ | <code>[illuminance](#illuminance)</code> | — | <code>[luminous_<wbr>flux](#luminous_flux) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/illuminance.md) |
| <span id="inductance"></span><code>inductance</code> | Real | ET²Q⁻² | ✓ | <code>[inductance](#inductance)</code> | — | <code>pow<2>([duration](#duration)) * <wbr>[energy](#energy) / <wbr>pow<2>([electric_<wbr>charge](#electric_charge))</code> | [view](../hierarchies/inductance_hep.md) |
| <span id="length"></span><code>length</code> | Real | L | ✓ | <code>[length](#length)</code> | — | — | [view](../hierarchies/length_hep.md) |
| <span id="luminous_flux"></span><code>luminous_<wbr>flux</code> | Real | α²I | ✓ | <code>[luminous_<wbr>flux](#luminous_flux)</code> | — | <code>[luminous_<wbr>intensity](#luminous_intensity) * <wbr>[solid_<wbr>angle](#solid_angle)</code> | [view](../hierarchies/luminous_flux.md) |
| <span id="luminous_intensity"></span><code>luminous_<wbr>intensity</code> | Real | I | ✓ | <code>[luminous_<wbr>intensity](#luminous_intensity)</code> | — | — | [view](../hierarchies/luminous_intensity_hep.md) |
| <span id="magnetic_field"></span><code>magnetic_<wbr>field</code> | Real | ETQ⁻¹L⁻² | ✓ | <code>[magnetic_<wbr>field](#magnetic_field)</code> | — | <code>[duration](#duration) * <wbr>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/magnetic_field.md) |
| <span id="magnetic_flux"></span><code>magnetic_<wbr>flux</code> | Real | ETQ⁻¹ | ✓ | <code>[magnetic_<wbr>flux](#magnetic_flux)</code> | — | <code>[duration](#duration) * <wbr>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge)</code> | [view](../hierarchies/magnetic_flux_hep.md) |
| <span id="mass"></span><code>mass</code> | Real | ET²L⁻² | ✓ | <code>[mass](#mass)</code> | — | <code>[energy](#energy) * <wbr>pow<2>([duration](#duration)) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/mass_hep.md) |
| <span id="power"></span><code>power</code> | Real | ET⁻¹ | ✓ | <code>[power](#power)</code> | — | <code>[energy](#energy) / <wbr>[duration](#duration)</code> | [view](../hierarchies/power_hep.md) |
| <span id="pressure"></span><code>pressure</code> | Real | EL⁻³ | ✓ | <code>[pressure](#pressure)</code> | — | <code>[energy](#energy) / <wbr>pow<3>([length](#length))</code> | [view](../hierarchies/pressure_hep.md) |
| <span id="solid_angle"></span><code>solid_<wbr>angle</code> | Real | α² | ✓ | <code>[solid_<wbr>angle](#solid_angle)</code> | — | <code>pow<2>([angle](#angle))</code> | [view](../hierarchies/solid_angle_hep.md) |
| <span id="temperature"></span><code>temperature</code> | Real | Θ | ✓ | <code>[temperature](#temperature)</code> | — | — | [view](../hierarchies/temperature.md) |
| <span id="volume"></span><code>volume</code> | Real | L³ | ✓ | <code>[volume](#volume)</code> | — | <code>pow<3>([length](#length))</code> | [view](../hierarchies/volume_hep.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="ampere"></span><code>ampere</code> | A | — | <code>[coulomb](#coulomb) / <wbr>[second](#second)</code> |
| <span id="angstrom"></span><code>angstrom</code> | Å (A) | — | <code>mag_power<10, -10> * <wbr>[meter](#meter)</code> |
| <span id="astronomical_unit"></span><code>astronomical_<wbr>unit</code> | au | — | <code>mag<149'597'870'700> * <wbr>[meter](#meter)</code> |
| <span id="atmosphere"></span><code>atmosphere</code> | atm | — | <code>mag<101'325> * <wbr>[pascal](#pascal)</code> |
| <span id="bar"></span><code>bar</code> | bar | — | <code>mag_power<10, 5> * <wbr>[pascal](#pascal)</code> |
| <span id="barn"></span><code>barn</code> | b | — | <code>mag_power<10, -28> * <wbr>square([meter](#meter))</code> |
| <span id="becquerel"></span><code>becquerel</code> | Bq | `Bq` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [activity](#activity)</code> |
| <span id="candela"></span><code>candela</code> | cd | — | <code>kind: [luminous_<wbr>intensity](#luminous_intensity)</code> |
| <span id="coulomb"></span><code>coulomb</code> | C | — | <code>mag_ratio<1'602'176'634, 1'000'000'000> * <wbr>[eplus](#eplus)</code> |
| <span id="curie"></span><code>curie</code> | Ci | `Ci` | <code>mag_ratio<37, 10> * <wbr>mag_power<10, 10> * <wbr>[becquerel](#becquerel)</code> |
| <span id="degree"></span><code>degree</code> | ° (deg) | `deg` | <code>mag_ratio<1, 180> * <wbr>[π](core.md#π) * <wbr>[radian](#radian)</code> |
| <span id="electronvolt"></span><code>electronvolt</code> | eV | `eV` | <code>kind: [energy](#energy)</code> |
| <span id="eplus"></span><code>eplus</code> | e⁺ (e+) | — | <code>kind: [electric_<wbr>charge](#electric_charge)</code> |
| <span id="farad"></span><code>farad</code> | F | — | <code>[coulomb](#coulomb) / <wbr>[volt](#volt)</code> |
| <span id="fermi"></span><code>fermi</code> | — | — | <code>[si::femto](si.md#femto)<[meter](#meter)></code> |
| <span id="gauss"></span><code>gauss</code> | G | — | <code>mag_power<10, -4> * <wbr>[tesla](#tesla)</code> |
| <span id="gram"></span><code>gram</code> | g | `g` | <code>mag_ratio<1, 1000> * <wbr>[joule](#joule) * <wbr>square([second](#second)) / <wbr>square([meter](#meter))</code> |
| <span id="gray"></span><code>gray</code> | Gy | — | <code>[joule](#joule) / <wbr>[si::kilo](si.md#kilo)<[gram](#gram)><br>kind: [absorbed_<wbr>dose](#absorbed_dose)</code> |
| <span id="halfpi"></span><code>halfpi</code> | — | — | <code>mag_ratio<1, 2> * <wbr>[pi](#pi)</code> |
| <span id="henry"></span><code>henry</code> | H | — | <code>[weber](#weber) / <wbr>[ampere](#ampere)</code> |
| <span id="hertz"></span><code>hertz</code> | Hz | — | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [frequency](#frequency)</code> |
| <span id="joule"></span><code>joule</code> | J | — | <code>[electronvolt](#electronvolt) * <wbr>[eplus](#eplus) / <wbr>[coulomb](#coulomb)</code> |
| <span id="kelvin"></span><code>kelvin</code> | K | — | <code>kind: [temperature](#temperature)</code> |
| <span id="liter"></span><code>liter</code> | L | `L` | <code>cubic([si::deci](si.md#deci)<[meter](#meter)>)</code> |
| <span id="lumen"></span><code>lumen</code> | lm | — | <code>[candela](#candela) * <wbr>[steradian](#steradian)</code> |
| <span id="lux"></span><code>lux</code> | lx | — | <code>[lumen](#lumen) / <wbr>square([meter](#meter))</code> |
| <span id="meter"></span><code>meter</code> | m | `m` | <code>kind: [length](#length)</code> |
| <span id="mole"></span><code>mole</code> | mol | — | <code>kind: [amount_<wbr>of_<wbr>substance](#amount_of_substance)</code> |
| <span id="newton"></span><code>newton</code> | N | — | <code>[joule](#joule) / <wbr>[meter](#meter)</code> |
| <span id="ohm"></span><code>ohm</code> | Ω (ohm) | — | <code>[volt](#volt) / <wbr>[ampere](#ampere)</code> |
| <span id="parsec"></span><code>parsec</code> | pc | `pc` | <code>[astronomical_<wbr>unit](#astronomical_unit) / <wbr>(mag_ratio<1, 3600> * <wbr>[degree](#degree))</code> |
| <span id="pascal"></span><code>pascal</code> | Pa | — | <code>[newton](#newton) / <wbr>square([meter](#meter))</code> |
| <span id="perCent"></span><code>perCent</code> | — | — | alias to [mp_<wbr>units::percent](core.md#percent) |
| <span id="perMillion"></span><code>perMillion</code> | — | — | alias to [mp_<wbr>units::parts_<wbr>per_<wbr>million](core.md#parts_per_million) |
| <span id="perThousand"></span><code>perThousand</code> | — | — | alias to [mp_<wbr>units::per_<wbr>mille](core.md#per_mille) |
| <span id="pi"></span><code>pi</code> | — | — | alias to ::[π](core.md#π) |
| <span id="radian"></span><code>radian</code> | rad | `rad` | <code>kind: [angle](#angle)</code> |
| <span id="second"></span><code>second</code> | s | `s` | <code>kind: [duration](#duration)</code> |
| <span id="steradian"></span><code>steradian</code> | sr | `sr` | <code>square([radian](#radian))</code> |
| <span id="tesla"></span><code>tesla</code> | T | — | <code>[weber](#weber) / <wbr>square([meter](#meter))</code> |
| <span id="twopi"></span><code>twopi</code> | — | — | <code>mag<2> * <wbr>[pi](#pi)</code> |
| <span id="volt"></span><code>volt</code> | V | — | <code>[electronvolt](#electronvolt) / <wbr>[eplus](#eplus)</code> |
| <span id="watt"></span><code>watt</code> | W | — | <code>[joule](#joule) / <wbr>[second](#second)</code> |
| <span id="weber"></span><code>weber</code> | Wb | — | <code>[volt](#volt) * <wbr>[second](#second)</code> |

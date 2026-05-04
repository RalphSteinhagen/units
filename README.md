<!-- markdownlint-disable MD041 -->
<!-- markdownlint-disable-next-line MD033 -->
<img align="right" height=135px src="docs/assets/images/mp-units-color.svg" alt="logo">

[![License](https://img.shields.io/github/license/mpusz/mp-units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-conan.yml)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-test-package-cmake.yml)
[![clang-tidy CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-clang-tidy.yml?branch=master&label=clang-tidy%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-clang-tidy.yml)
[![Freestanding CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-freestanding.yml?branch=master&label=Freestanding%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-freestanding.yml)
[![Formatting CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-formatting.yml?branch=master&label=Formatting%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-formatting.yml)
[![Documentation](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/documentation.yml?branch=master&label=Documentation)](https://github.com/mpusz/mp-units/actions?query=workflow%3ADocumentation+branch%3Amaster)

[![Conan Center](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-2.5.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/2.5.0)


# `mp-units` – The Domain-Correct Quantities and Units Library for C++


## 🎯 Overview

**`mp-units`** is the only Modern C++ (C++20 and later) library providing the full
spectrum of compile‑time safety for domain-specific quantities and units — from
dimensional analysis to quantity kind safety — built on the ISO 80000 International
System of Quantities (ISQ). It is the leading candidate for C++29 standardization
([P3045](https://wg21.link/p3045)) — your chance to shape the future of C++.

```cpp
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Compile-time dimensional analysis — zero runtime overhead
static_assert(1 * km / (1 * s) == 1000 * m / s);

// Function signatures encode domain/physics, not just dimensions
void calculate_trajectory(quantity<isq::kinetic_energy[J]> e);

int main()
{
  quantity<isq::potential_energy[J]> Ep = 42 * J;
  quantity<isq::kinetic_energy[J]>   Ek = 123 * J;
  calculate_trajectory(Ek);         // ✅ correct
  // calculate_trajectory(Ep);      // ❌ potential energy ≠ kinetic energy (both in J)

  // quantity<Gy> q = 42 * Sv;      // ❌ absorbed dose ≠ dose equivalent (both J/kg)
}
```

[![Try it live on Compiler Explorer](https://img.shields.io/badge/Try_live_on-Compiler_Explorer-black?style=for-the-badge&logo=compilerexplorer&labelColor=black&color=67C52A)](https://godbolt.org/z/YWErT7ofs)

### What Sets mp-units Apart?

Beyond standard dimensional analysis and automatic unit conversions, **mp-units** provides
safety levels available in no other C++ library:

- 🥇 **The only C++ library with Quantity Kind Safety** — Distinguishes quantities that
  share the same dimension but represent fundamentally different physical concepts:
  _frequency_ (Hz) ≠ _radioactive activity_ (Bq), _absorbed dose_ (Gy) ≠ _dose equivalent_
  (Sv), _plane angle_ (rad) ≠ _solid angle_ (sr). Dimensional analysis alone cannot catch
  these errors — **mp-units** prevents them at compile time.

- 🥇 **The only library implementing ISO 80000 (ISQ)** — Built on the International System
  of Quantities, functions can require _specific_ quantities: `isq::height` (not just any
  `isq::length`), `isq::kinetic_energy` (not just any `isq::energy`). The physics of your
  domain becomes part of the type system.

- 🥇 **Strongly-Typed Numerics for Any Domain** — The quantity framework extends beyond
  physics: define semantically distinct types for item counts, financial values, identifiers,
  or any numeric abstraction that should never be silently mixed at compile time.


## 💡 Examples

**mp-units** provides an expressive, readable API that feels natural to write while
catching entire classes of bugs at compile time.

### Unit Arithmetic

Here's a taste of what **mp-units** can do:

```cpp
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// simple numeric operations
static_assert(10 * km / 2 == 5 * km);

// conversions to common units
static_assert(1 * h == 3600 * s);
static_assert(1 * km + 1 * m == 1001 * m);

// derived quantities
static_assert(1 * km / (1 * s) == 1000 * m / s);
static_assert(2 * km / h * (2 * h) == 4 * km);
static_assert(2 * km / (2 * km / h) == 1 * h);

static_assert(2 * m * (3 * m) == 6 * m2);

static_assert(10 * km / (5 * km) == 2 * one);

static_assert(1000 / (1 * s) == 1 * kHz);
```

[![Try it live on Compiler Explorer](https://img.shields.io/badge/Try_live_on-Compiler_Explorer-black?style=for-the-badge&logo=compilerexplorer&labelColor=black&color=67C52A)](https://godbolt.org/z/fT1r4sohs)

### Modern C++ Design

The library makes extensive use of **C++20 features** (concepts, class types as NTTPs, etc.).
This enables powerful yet easy‑to‑use interfaces while performing all conversions and dimensional
analysis at compile time — without sacrificing runtime performance or accuracy.
The example below showcases ISQ quantity types, mixed unit systems, and rich text formatting:

```cpp
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <format>
#include <iomanip>
#include <iostream>
#include <print>

using namespace mp_units;

constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto d,
                                                QuantityOf<isq::duration> auto t)
{
  return d / t;
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  constexpr quantity v1 = 110 * km / h;
  constexpr quantity v2 = 70 * mph;
  constexpr quantity v3 = avg_speed(220. * isq::distance[km], 2 * h);
  constexpr quantity v4 = avg_speed(isq::distance(140. * mi), 2 * h);
  constexpr quantity v5 = v3.in(m / s);
  constexpr quantity v6 = value_cast<m / s>(v4);
  constexpr quantity v7 = value_cast<int>(v6);

  std::cout << v1 << '\n';                                        // 110 km/h
  std::cout << std::setw(10) << std::setfill('*') << v2 << '\n';  // ***70 mi/h
  std::cout << std::format("{:*^10}\n", v3);                      // *110 km/h*
  std::println("{:%N in %U of %D}", v4);                          // 70 in mi/h of LT⁻¹
  std::println("{::N[.2f]}", v5);                                 // 30.56 m/s
  std::println("{::N[.2f]U[dn]}", v6);                            // 31.29 m⋅s⁻¹
  std::println("{:%N}", v7);                                      // 31
}
```

[![Try it live on Compiler Explorer](https://img.shields.io/badge/Try_live_on-Compiler_Explorer-black?style=for-the-badge&logo=compilerexplorer&labelColor=black&color=67C52A)](https://godbolt.org/z/rYq7cfdxY)


## ✅ Key Features

**Safety**
- Quantity kind safety: same dimension, different meaning → compile-time error
- Affine space strong types (`quantity` and `quantity_point`)
- Value-preserving conversions

**Performance**
- All dimensional analysis at compile time — zero runtime overhead
- Performance on par with (sometimes even better than) fundamental types

**User Experience**
- Optimized for readable, actionable compilation errors
- Expressive, composable unit expressions

**Feature Rich**
- Systems of Quantities *and* Units; scalar, vector, and tensor quantities
- Affine space, natural units, strong angular system
- Highly adjustable text output formatting

**Easy to Extend**
- Custom dimensions, quantities, and units in a single line of code

**Low Adoption Cost**
- No external dependencies · macro-free API · C++20 modules-ready · freestanding-capable

→ [Full feature overview](https://mpusz.github.io/mp-units/latest/getting_started/about/)


## 📚 Documentation

Extensive project documentation covers everything from getting started to advanced usage:

- **Installation instructions** – Get up and running quickly
- **Detailed user's guide** – Comprehensive usage documentation
- **Design rationale** – Understanding the architectural decisions
- **API reference** – Complete technical documentation
- **Tutorials** – Step-by-step learning resources
- **Workshops** – Hands-on practice exercises
- **Examples** – Real-world usage demonstrations

→ [Explore the full documentation](https://mpusz.github.io/mp-units)


## 🔍 Try It Out

For **advanced development** or **contributions**, we provide a fully configured cloud
development environment with [GitHub Codespaces](https://docs.github.com/en/codespaces):

[![Open in GitHub Codespaces](https://img.shields.io/badge/Open_in-GitHub_Codespaces-blue?style=for-the-badge&logo=github&labelColor=black&color=2088FF)](https://codespaces.new/mpusz/mp-units)

**Alternatives:**
1. Navigate to the repository → **"Code"** → **"Codespaces"** → **"Create codespace on master"**
2. Use the pre‑configured devcontainer and Docker image manually in your IDE

For detailed environment documentation, see [`.devcontainer/README.md`](.devcontainer/README.md).


## 🚀 Help Shape the Future of C++

**`mp-units` is a candidate for ISO standardization for C++29** — the future of dimensional
analysis in C++! The technical case is documented in:

- [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935)
- [P2980: A motivation, scope, and plan for a quantities and units library](https://wg21.link/p2980)
- [P3045: Quantities and units library](https://wg21.link/p3045)

> 🤝 **We are actively seeking organizations and individuals interested in**
> **field‑trialing the library!**

**Your experience matters.** Real-world testimonials demonstrate value to the ISO C++ Committee
and help potential adopters decide. Whether you're using **mp-units** in **production**,
**research**, or **education**:

- **Organizations**: Share your production deployments and success stories
- **Academics**: Report research applications and teaching experiences
- **Developers**: Tell us about your innovative use cases and benefits

[![Share Experience](https://img.shields.io/badge/Share_Your-Usage_Experience-blue?style=for-the-badge&labelColor=black&label=🌟%20Share%20Your)](https://github.com/mpusz/mp-units/issues/new?template=usage_experience.yml)


## 🤝 Contributors

**mp-units** is made possible by our amazing community of contributors! 💪

[![Contributors](https://img.shields.io/github/contributors/mpusz/mp-units?style=for-the-badge&logo=github&labelColor=black&color=blue)](https://github.com/mpusz/mp-units/graphs/contributors)
[![Commits](https://img.shields.io/github/commit-activity/m/mpusz/mp-units?style=for-the-badge&logo=git&labelColor=black&color=green)](https://github.com/mpusz/mp-units/pulse)
[![Stars](https://img.shields.io/github/stars/mpusz/mp-units?style=for-the-badge&label=⭐%20Stars&labelColor=black&color=gold)](https://github.com/mpusz/mp-units/stargazers)

### 🏆 Core Team

- **[Mateusz Pusz](https://github.com/mpusz)** – Project founder and lead
- **[Johel Ernesto Guerrero Peña](https://github.com/JohelEGP)** – Core maintainer
- **[Chip Hogg](https://github.com/chiphogg)** – Core maintainer

### 🙏 All Contributors

We appreciate **every contribution**, from code to documentation to community support!

🌟 See our [**Contributors Page**](CONTRIBUTORS.md) for the complete list and recognition details.

> **Ready to contribute?** Check out our
> [**Contributing Guide**](https://mpusz.github.io/mp-units/latest/getting_started/contributing/)
> to get started! 🚀


## 💝 Support the Project

**mp-units** is developed as open source with the ambitious goal of C++29 standardization.
Your support helps maintain development momentum and accelerate standardization efforts!

**Ways to support:**
- ⭐ **Star the repository** – Show your appreciation and help others discover **mp-units**
- 💰 **Become a sponsor** – Financial support enables continued development

  [![Sponsor](https://img.shields.io/badge/Sponsor-GitHub_Sponsors-pink?style=for-the-badge&logo=githubsponsors&labelColor=black&color=EA4AAA)](https://github.com/sponsors/mpusz)

- 📢 **Share your success story** – Help demonstrate real-world value for standardization
  and other potential users
- 🤝 **Contribute** – Code, documentation, feedback, and community support

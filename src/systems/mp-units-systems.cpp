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

module;

#include <mp-units/bits/core_gmf.h>
#include <chrono>

export module mp_units.systems;

export import mp_units.core;
#ifdef MP_UNITS_IMPORT_STD
import std;
#endif

#define MP_UNITS_IN_MODULE_INTERFACE

#include <mp-units/systems/angular.h>
#include <mp-units/systems/cgs.h>
#include <mp-units/systems/hep.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/iec.h>
#include <mp-units/systems/iec80000.h>
#include <mp-units/systems/imperial.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/isq_angle.h>
#if MP_UNITS_API_NATURAL_UNITS
#include <mp-units/systems/natural.h>
#endif
#include <mp-units/systems/si.h>
#include <mp-units/systems/typographic.h>
#include <mp-units/systems/usc.h>

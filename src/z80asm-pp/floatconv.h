//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>
using namespace std;

enum class FloatType {
    IEEE_Single, IEEE_Double, ZX81, ZX,
};

vector<uint8_t> float_to_bytes(double value, FloatType float_type = FloatType::IEEE_Single);

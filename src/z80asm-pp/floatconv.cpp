//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "floatconv.h"
#include <cassert>
#include <cmath>

union myfloat {
    float   value;
    uint8_t bytes[4];
};
static_assert(sizeof(float) == 4, "expected 4 bytes");
static_assert(sizeof(myfloat) == 4, "expected 4 bytes");

union mydouble {
    double	 value;
    uint8_t  bytes[8];
    uint64_t raw;	// 1 bit sign, 11 bits exponent, 52 bits mantissa, bias 1023
};
static const int mydouble_exp_bias = 1023;
static_assert(sizeof(double) == 8, "expected 8 bytes");
static_assert(sizeof(mydouble) == 8, "expected 8 bytes");

static inline bool is_big_endian() {
    unsigned x = 0x76543210;
    char* c = (char*)&x;
    if (*c == 0x76)
        return true;
    else if (*c == 0x10)
        return false;
    else {
        assert(0);
        return false;	// not reached
    }
}

static vector<uint8_t> float_to_ieee_single(double value) {
    vector<uint8_t>	out;
    myfloat f;

    f.value = static_cast<float>(value);
    if (is_big_endian()) {	// invert byte order
        for (int i = (int)sizeof(f) - 1; i >= 0; i--)
            out.push_back(f.bytes[i]);
    }
    else {
        for (int i = 0; i < (int)sizeof(f); i++)
            out.push_back(f.bytes[i]);
    }

    return out;
}

static vector<uint8_t> float_to_ieee_double(double value) {
    vector<uint8_t>	out;
    mydouble f;
    f.value = value;

    if (is_big_endian()) {	// invert byte order
        for (int i = (int)sizeof(f) - 1; i >= 0; i--)
            out.push_back(f.bytes[i]);
    }
    else {
        for (int i = 0; i < (int)sizeof(f); i++)
            out.push_back(f.bytes[i]);
    }

    return out;
}

// 1 byte exponent
// 4 bytes mantissa, with first bit replaced by sign bit
static vector<uint8_t> float_to_zx81(double value) {
    vector<uint8_t>	out;
    mydouble f;
    f.value = value;

    // convert to zx81 format
    uint64_t exponent = ((f.raw >> 52) & 0x7ff) - mydouble_exp_bias + 129;
    uint64_t mantissa = (f.raw >> (52 - 32 + 1));	// align first bit to bit30, bit31 is sign
    if (value < 0.0)
        mantissa |= 1LL << 31;
    else
        mantissa &= ~(1LL << 31);

    // return
    out.push_back(exponent & 0xff);
    for (size_t i = 0; i < 4; i++) {
        out.push_back((mantissa >> 24) & 0xff);
        mantissa <<= 8;
    }
    return out;
}

// if integer: 0, 0, low, high, 0 (positive) | 0, 255, low, high, 0 (negative)
// else: same as zx81
static vector<uint8_t> float_to_zx(double value) {
    if (floor(value) == value && fabs(value) <= 65535.0) {
        int ivalue = static_cast<int>(floor(value));
        vector<uint8_t>	out;

        out.push_back(0);
        if (value >= 0.0)
            out.push_back(0);
        else
            out.push_back(255);
        out.push_back(ivalue & 0xff);
        out.push_back((ivalue >> 8) & 0xff);
        out.push_back(0);

        return out;
    }
    else
        return float_to_zx81(value);
}

vector<uint8_t> float_to_bytes(double value, FloatType float_type) {
    switch (float_type) {
    case FloatType::IEEE_Single: return float_to_ieee_single(value);
    case FloatType::IEEE_Double: return float_to_ieee_double(value);
    case FloatType::ZX81: return float_to_zx81(value);
    case FloatType::ZX: return float_to_zx(value);
    default:
        assert(0);
        return vector<uint8_t> {};	// not reached
    }
}

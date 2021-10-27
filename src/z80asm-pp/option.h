//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "floatconv.h"
#include "symtab.h"
#include <string>
#include <vector>
using namespace std;

struct Options {
    Macros			macros;			// global macros
    vector<string>  include_path;   // search path for includes
    bool            ucase;          // convert symbols to upper case
    bool			verbose;		// show what is happening
    FloatType		float_type{ FloatType::IEEE_Single };	// float format
};

string ident_change_case(const string& ident);

extern Options g_options;


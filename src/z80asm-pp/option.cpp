//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "option.h"
#include "utils.h"
using namespace std;

Options g_options;

string ident_change_case(const string& ident) {
    if (g_options.ucase)
        return str_toupper(ident);
    else
        return ident;
}

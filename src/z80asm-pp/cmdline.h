//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

void usage();
void parse_env_args(const string& envvar);
void parse_cmdline_arg(const string& arg);

//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "common.h"
#include "error.h"
#include "cmdline.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    try {
        if (argc == 1)
            usage();
        else {
            parse_env_args(OPTIONS_ENV);
            for (int i = 1; i < argc; ++i)
                parse_cmdline_arg(argv[i]);
        }
    }
    catch (const Error& e) {
        cerr << e;
    }
    Error::report_exit();
}

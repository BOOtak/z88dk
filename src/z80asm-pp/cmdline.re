//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "cmdline.h"
#include "common.h"
#include "error.h"
#include "option.h"
#include "preproc.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
using namespace std;

/*!re2c
	re2c:define:YYCTYPE = char;
	re2c:yyfill:enable = 0;
	re2c:indent:top = 2;

	end 	= "\000";
	ident 	= [_a-zA-Z][_a-zA-Z0-9.]*;
	value	= [^\000]+;
	file	= [^\000-][^\000]*;
*/

void usage() {
    cout
            << "z80asm preprocessor (c) Paulo Custodio 2011-2021" << endl
            << endl
            << "Usage: " << PROG << " [options] files..." << endl
            << endl
            << "  options:" << endl
            << "  -Dvar[=value] define variable, default value is 1" << endl
            << "  -Idir         apppend directory to include path" << endl
            << "  -ucase        convert identifiers to upper case" << endl
            << "  -float=type   use type when converting floats" << endl
            << "                type: ieee32, ieee64, zx81, zx" << endl
            << "  -v            verbose" << endl;
}

void parse_env_args(const string& envvar) {
    const char* env_args = getenv(envvar.c_str());
    if (env_args) {
        // -v option was not read yet
        if (strstr(env_args, "-v"))
            cout << "Processing environment variable: "
                 << envvar << "=" << env_args << endl;
        istringstream ss{ env_args };
        string arg;
        while (ss >> arg)
            parse_cmdline_arg(arg);		// may throw
    }
}

void parse_cmdline_arg(const string& arg) {
    const char* YYCURSOR = arg.c_str(), * YYMARKER, * p1, * p2, * p3, * p4;
    const char* yyt1{ nullptr }, * yyt2{ nullptr };
    /*!re2c
    	"-D" @p1 ident @p2 "=" @p3 value @p4 end {
    				g_options.macros.add(
    					make_shared<Macro>(string(p1,p2), string(p3,p4)));
    				return;
    			}
    	"-D" @p1 ident @p2 end {
    				g_options.macros.add(
    					make_shared<Macro>(string(p1,p2), "1"));
    				return;
    			}

    	"-I" @p1 value @p2 end {
    				g_options.include_path.push_back(string(p1,p2));
    				return;
    			}

    	"-ucase" end	{ g_options.ucase = true; return; }

    	"-float=ieee32" end { g_options.float_type = FloatType::IEEE_Single; return; }
        "-float=ieee64" end { g_options.float_type = FloatType::IEEE_Double; return; }
        "-float=zx81" end	{ g_options.float_type = FloatType::ZX81; return; }
        "-float=zx" end		{ g_options.float_type = FloatType::ZX; return; }

    	"-v" end		{ g_options.verbose = true; return; }
    	"-"				{ goto error; }
    	@p1 file @p2 end{ preproc_file(string(p1,p2)); return; }
    	end				{ return; }
    */

error:
    throw Error(ECode::InvalidOption, arg);
}

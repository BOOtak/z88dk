//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "error.h"
#include "common.h"
#include "option.h"
#include <iostream>
using namespace std;

Location g_location;

int Error::m_count = 0;

static const char* error_msgs[] = {
#	define X(id, text)   text,
#	include "error.def"
};

Error::Error(Location location, ECode ecode, const string& arg)
    : m_location(location), m_ecode(ecode), m_arg(arg) {
    m_count++;
}

Error::Error(ECode ecode, const string& arg)
    : m_location(g_location), m_ecode(ecode), m_arg(arg) {
    m_count++;
}

void Error::put(ostream& os) const {
    put(os, "error", m_location, m_ecode, m_arg);
}

void Error::warning(ostream& os,
                    Location location, ECode ecode, const string& arg) {
    put(os, "warning", location, ecode, arg);
}

void Error::warning(ostream& os, ECode ecode, const string& arg) {
    warning(os, g_location, ecode, arg);
}

void Error::report_exit() {
    if (m_count == 0)
        exit(EXIT_SUCCESS);
    else {
		if (g_options.verbose) {
			cerr << PROG << " found " << m_count << " error";
			if (m_count != 1)
				cerr << "s";
			cerr << "." << endl;
		}
		exit(EXIT_FAILURE);
    }
}

void Error::put(ostream& os, const char* prefix,
                Location location, ECode ecode, const string& arg) {
    if (location.filename.empty())
        os << PROG << ": ";
    else
        os << location.filename << ":" << location.line_num << ": ";
    os << prefix << ": " << error_msgs[static_cast<int>(ecode)];
    if (!arg.empty())
        os << ": " << arg;
    os << endl;
}

ostream& operator<<(ostream& os, const Error& e) {
    e.put(os);
    return os;
}

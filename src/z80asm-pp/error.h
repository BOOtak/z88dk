//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

struct Location {
    string  filename;
    int     line_num{ 0 };

    Location(const string& filename = "", int line_num = 0)
        : filename(filename), line_num(line_num) {}
    void clear() { filename.clear(); line_num = 0; }
};

extern Location g_location;		// used by Error

enum class ECode {
#	define X(id, text)		id,
#	include "error.def"
};

// error exception
class Error {
public:
    Error(Location location, ECode ecode, const string& arg = "");
    Error(ECode ecode, const string& arg = "");

    Location location() const { return m_location; }
    ECode ecode() const { return m_ecode; }
    const string& arg() const { return m_arg; }
    void put(ostream& os) const;

    static int count() { return m_count; }
    static void inc_count() { m_count++; }
    static void warning(ostream& os,
                        Location location, ECode ecode, const string& arg = "");
    static void warning(ostream& os, ECode ecode, const string& arg = "");
    static void report_exit();

private:
    Location	m_location;
    ECode		m_ecode;
    string		m_arg;

    static int m_count;

    static void put(ostream& os, const char* prefix,
                    Location location, ECode ecode, const string& arg = "");
};

ostream& operator<<(ostream& os, const Error& e);

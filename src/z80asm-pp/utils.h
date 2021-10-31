//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

// std::filesystem from C++17
#include <filesystem>
namespace fs = std::filesystem;

// change case
string str_tolower(string str);
string str_toupper(string str);

// convert C-escape sequences
string str_compress_escapes(const string& in);

// check string ending
bool str_ends_with(const string& str, const string& ending);

// read lines with any EOL terminator
istream& safe_getline(istream& is, string& t);

// search file in path
template<typename T>
string search_file(const string& filename, const T& path) {
    if (fs::is_regular_file(filename))
        return filename;                        // file exists
    else {
        for (const auto& dir : path) {
            fs::path p = fs::path(dir) / filename;
            if (fs::is_regular_file(p))
                return p.generic_string();      // file exists in directory
        }

        return "";                              // empty string if not found
    }

}

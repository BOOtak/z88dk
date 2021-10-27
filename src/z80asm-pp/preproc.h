//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "error.h"
#include "expr.h"
#include "lex.h"
#include "symtab.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
using namespace std;

struct Line {
    Location    location;
    string      text;

    Line(const Location& location = g_location, const string& text = "")
        : location(location), text(text) {}
    void clear() { location.clear(); text.clear(); }
};

struct PreprocLevel {
	Line			line;
	Lexer		lexer;
	Macros		defines;					// #defines, -D, macro args
	bool			exitm_called{ false };

	PreprocLevel(Macros* parent = nullptr);
	void init(const Line& line);
	bool getline();

private:
	deque<Line>	m_lines;
};

struct PreprocFile {
    string		filename;
    ifstream		ifs;
    Location		location;

    PreprocFile(const string& filename, ifstream& ifs)
        : filename(filename)
        , ifs(move(ifs))
        , location(filename, 0) {}

	bool getline(Line& line);

private:
	PreprocLevel	 m_queue;

	bool getline_(Line& line);
};

class ExpandedText {
public:
	string text() const { return m_text; }
	bool got_error() const { return m_error; }
	void append(const string& str);
	void set_error(bool f) { m_error = f; }
private:
	string  m_text;
	bool    m_error{ false };
};

struct IfNest {
	Keyword keyword;
	bool  flag;
	bool  done_if;

	IfNest(Keyword keyword, bool flag) : keyword(keyword), flag(flag), done_if(false) {}
};

class Preproc {
public:
    Preproc(const string& filename);

    void parse();						// top level parse
    void parse(const string& filename);	// parse include

    void write();						// write to filename.i
    void write(ostream& os);

private:
    string				m_filename;
    list<PreprocFile>	m_files;			// input stack of files
	list<PreprocLevel>	m_levels;		// levels of macro expansion
	Macros				m_macros;		// opcode-like macros
	shared_ptr<Macro>	m_last_macro;	// last macro #defined
	Symtab				m_symtab;		// DEFC symbols
	vector<IfNest>		m_ifs;			// state of nested IFs
	int					m_defvars_value{ 0 };	// next defvars value
    list<Line>			m_output;       // parsed output

    string& filename() { return m_files.back().filename; }
    ifstream& ifs() { return m_files.back().ifs; }
    Location& location() { return m_files.back().location; }
    Line& line() { return m_levels.back().line; }
    Lexer& lexer() { return m_levels.back().lexer; }
	Macros& defines() { return m_levels.back().defines; }
	Macros& defines_base() { return m_levels.front().defines; }

	bool getline();						// get next line from file or macro expansion
    void do_parse();						// parse top of m_files
    bool parse_preproc();				// parse preproc commands

	bool check_if();
	bool check_ifdef();
	bool check_ifndef();
	bool check_elif();
	bool check_elifdef();
	bool check_elifndef();
	bool check_else();
	bool check_endif();
	bool check_include();
	bool check_define();
	bool check_defcont();
	bool check_undef();
	bool check_defc_equ();
	bool check_defl();
	bool check_macro();
	bool check_local();
	bool check_exitm();
	bool check_rept();
	bool check_reptc();
	bool check_repti();
	bool check_defgroup();
	bool check_defvars();
	bool check_binary();
	bool check_line();
	bool check_c_line();
	bool check_macro_call();	

	void expect_eol();
	TType expect_comma_or_rparens();
	void expect_eq();
	void expect_comma();
	string expect_ident();
	bool expect_if_flag();
	bool expect_idef_defined();
	int expect_const_expr(bool at_end = true);
	Value expect_expr(bool at_end = true);
	string expect_macro_body();
	string expect_rept_body();
	TType next_token_getline();
	int expect_size_id();
	string expect_string();

	void do_label();
    void do_include();
	void do_define();
	void do_defcont();
	void do_undef();
	void do_defc(const string& name);
	void do_defl(const string& name);
	void do_if();
	void do_ifdef();
	void do_ifndef();
	void do_elif();
	void do_elifdef();
	void do_elifndef();
	void do_else();
	void do_endif();
	void do_macro(const string& name);
	void do_macro_call(shared_ptr<Macro> macro);
	void do_local();
	void do_exitm();
	void do_rept();
	void do_reptc();
	void do_repti();
	void do_defgroup();
	void do_defvars();
	void do_binary();
	void do_line(bool is_c_line);

	ExpandedText expand(Lexer& lexer, Macros& defines);
	void expand_ident(ExpandedText& out, const string& ident, Lexer& lexer, Macros& defines);
	ExpandedText expand_define_call(const string& name, Lexer& lexer, Macros& defines);
	string collect_param(Lexer& lexer);
	vector<string> collect_macro_params(Lexer& lexer);
	bool ifs_active() const;
};

void preproc_file(const string& filename);
int next_id();
string unique_name(const string& name);


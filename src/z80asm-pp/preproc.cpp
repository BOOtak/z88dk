//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "preproc.h"
#include "floatconv.h"
#include "lex.h"
#include "option.h"
#include "utils.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
using namespace std;

static string concat(const string& s1, const string& s2) {
    if (s1.empty() || s2.empty())
        return s1 + s2;
    else if (str_ends_with(s1, "##"))   // cpp-style concatenation
        return s1.substr(0, s1.length() - 2) + s2;
    else if (isspace(s1.back()) || isspace(s2.front()))
        return s1 + s2;
    else if (isident(s1.back()) && isident(s2.front()))
        return s1 + " " + s2;
    else if (s1.back() == '$' && isxdigit(s2.front()))
        return s1 + " " + s2;
    else if ((s1.back() == '%' || s1.back() == '@') &&
             (isdigit(s2.front()) || s2.front() == '"'))
        return s1 + " " + s2;
    else if ((s1.back() == '&' && s2.front() == '&') ||
             (s1.back() == '|' && s2.front() == '|') ||
             (s1.back() == '^' && s2.front() == '^') ||
             (s1.back() == '*' && s2.front() == '*') ||
             (s1.back() == '<' && (s2.front() == '=' || s2.front() == '<' || s2.front() == '>')) ||
             (s1.back() == '>' && (s2.front() == '=' || s2.front() == '>')) ||
             (s1.back() == '=' && s2.front() == '=') ||
             (s1.back() == '!' && s2.front() == '=') ||
             (s1.back() == '#' && s2.front() == '#'))
        return s1 + " " + s2;
    else
        return s1 + s2;
}

static string str_to_byte_list(const string& str) {
    string out;
    if (str.empty())
        out = concat(out, "\"\"");
    else {
        for (size_t i = 0; i < str.size(); i++) {
            if (i > 0)
                out = concat(out, ",");
            out = concat(out, to_string(static_cast<unsigned>(str[i])));
        }
    }
    return out;
}

static string float_to_byte_list(double value) {
    vector<uint8_t> bytes = float_to_bytes(value, g_options.float_type);
    string out, sep;
    for (auto byte : bytes) {
        out = concat(out, sep);
        out = concat(out, to_string(byte));
        sep = ",";
    }
    return out;
}

PreprocLevel::PreprocLevel(Macros* parent)
	: defines(parent) {
	init(Line());
}

void PreprocLevel::init(const Line& line) {
	try {
		string text{ line.text };
		lexer.clear();
		lexer.set(text);

		if (lexer.peek().is(TType::Hash)) {
			// if line starts with '#' dont split on '\\'
			m_lines.push_back(line);
		}
		else {
			// split line by '\\' and '\n'
			size_t start_col = 0;
			for (size_t i = 0; i < lexer.size(); i++) {
				if (lexer[i].is(TType::Backslash, TType::Newline)) {
					Line this_line{ line.location };
					this_line.text = string(
						text.c_str() + start_col,
						text.c_str() + lexer[i].col);
					m_lines.push_back(this_line);
					start_col = lexer[i].col + 1;
				}
			}
			if (start_col != text.length()) {
				Line this_line{ line.location };
				this_line.text = string(
					text.c_str() + start_col,
					text.c_str() + text.length());
				m_lines.push_back(this_line);
			}
		}
	}
	catch (const Error& e) {
		cerr << e;				// show error and continue parsing
	}
}

bool PreprocLevel::getline() {
	line.clear();
	lexer.clear();
	if (!m_lines.empty()) {			// have lines in queue
		line = m_lines.front();
		m_lines.pop_front();

		g_location = line.location;
		lexer.set(line.text);
		return true;
	}
	else
		return false;
}

bool PreprocFile::getline(Line& line) {
	while (true) {
		if (m_queue.getline()) {
			line = m_queue.line;
			return true;
		}
		else if (getline_(line)) 
			m_queue.init(line);
		else
			return false;
	}
}

bool PreprocFile::getline_(Line& line) {
	string text;

	// read line
	if (!safe_getline(ifs, text))
		return false;

	location.line_num++;
	line.location = g_location = location;
	line.text = text;

	// if line ends with backslash, concatenate with next line
	while (!line.text.empty() && line.text.back() == '\\') {
		line.text.pop_back();       // remove '\\'

		if (!safe_getline(ifs, text))
			break;					// eof, no next line

		location.line_num++;
		line.text += string(" ") + text;
	}
	return true;
}

void ExpandedText::append(const string& str) {
	m_text = concat(m_text, str);
}

Preproc::Preproc(const string& filename)
	: m_filename(filename) {
	m_levels.emplace_back(&g_options.macros);
}

void Preproc::parse() {
    parse(m_filename);
}

void Preproc::parse(const string& filename) {
    // check if file exists
    string filename_path{ search_file(filename, g_options.include_path) };
    if (filename_path.empty())
        throw Error(ECode::FileNotFound, filename);

    if (m_files.empty())		// top level
        m_filename = filename_path;

    // check for recursive includes
    auto found = find_if(m_files.begin(), m_files.end(),
    [filename_path](PreprocFile & f) {
        return f.filename == filename_path;
    });
    if (found != m_files.end())
        throw Error(ECode::RecursiveInclude, filename_path);

    // check if can open file
    ifstream ifs{ filename_path };
    if (!ifs.is_open())
        throw Error(ECode::OpenFile, filename_path);

    // add directory of input file to include path
    // to allow relative includes
    string parent_dir = fs::path(filename_path).parent_path().generic_string();
    if (parent_dir.empty())
        parent_dir = ".";
    g_options.include_path.push_back(parent_dir);

    // create new scope
    m_files.emplace_back(filename_path, ifs);

    // do parse
    do_parse();

    // remove scope
    m_files.pop_back();

    // remove directory from include path
    g_options.include_path.pop_back();
}

void Preproc::write() {
    string out_file = fs::path(m_filename).replace_extension(".i").generic_string();
    ofstream ofs(out_file);
    if (!ofs.is_open())
        throw Error(ECode::CannotWriteFile, out_file);
    else
        write(ofs);
}

void Preproc::write(ostream& os) {
    Location here;
    for (auto& line : m_output) {   // split each output line by embedded newlines
        istringstream iss{ line.text };
        string text;
        while (std::getline(iss, text, '\n')) {
            // output LINE ddd,"file" if needed
            if (line.location.filename != here.filename) {
                os << "line " << line.location.line_num
                   << ",\"" << line.location.filename << "\"" << endl;
                here = line.location;
            }
            else if (line.location.line_num < here.line_num) {
                os << "line " << line.location.line_num << endl;
                here = line.location;
            }
            else {
                while (line.location.line_num > here.line_num) {
                    os << endl;
                    here.line_num++;
                }
            }

            // output text
            os << text << endl;
            here.line_num++;
        }
    }
}

bool Preproc::getline() {
	Line line;

	while (true) {
		assert(!m_levels.empty());
		if (m_levels.back().getline())
			return true;
		else if (m_levels.size() > 1)
			m_levels.pop_back();		// drop one level and continue
		else if (m_files.back().getline(line)) 
			m_levels.back().init(line);
		else {
			// end of input
			if (!m_ifs.empty())
				throw Error(ECode::StructNesting);
			return false;
		}
	}
}

void Preproc::do_parse() {
    while (getline()) {
        try {
            if (parse_preproc()) {
                // was a preprocessor command or ifs_active() was false
            }
            else {
				lexer().rewind();
				ExpandedText expanded = expand(lexer(), defines());
				if (!expanded.got_error())
					line().text = expanded.text();
                m_output.push_back(line());
            }
        }
        catch (const Error& e) {
            cerr << e;			// show error and continue parsing
        }
    }
}

bool Preproc::parse_preproc() {
	// do these irrespective of ifs_active()
	if (check_if()) return true;
	if (check_ifdef()) return true;
	if (check_ifndef()) return true;
	if (check_elif()) return true;
	if (check_elifdef()) return true;
	if (check_elifndef()) return true;
	if (check_else()) return true;
	if (check_endif()) return true;

	if (!ifs_active()) return true;

	// do these only if ifs_active()
	if (check_include()) return true;
	if (check_define()) return true;
	if (check_defcont()) return true;
	if (check_undef()) return true;
	if (check_defc_equ()) return true;
	if (check_defl()) return true;
	if (check_macro()) return true;
	if (check_local()) return true;
	if (check_exitm()) return true;
	if (check_rept()) return true;
	if (check_reptc()) return true;
	if (check_repti()) return true;
	if (check_defgroup()) return true;
	if (check_defvars()) return true;
	if (check_binary()) return true;
	if (check_line()) return true;
	if (check_c_line()) return true;

	// last check
	if (check_macro_call()) return true;

	return false;
}

bool Preproc::check_if() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::IF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_if();
		return true;
	}
	else if (l[0].is(Keyword::IF)) {
		lexer().next();
		do_if();
		return true;
	}
	else
		return false;
}

bool Preproc::check_ifdef() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::IFDEF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_ifdef();
		return true;
	}
	else if (l[0].is(Keyword::IFDEF)) {
		lexer().next();
		do_ifdef();
		return true;
	}
	else
		return false;
}

bool Preproc::check_ifndef() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::IFNDEF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_ifndef();
		return true;
	}
	else if (l[0].is(Keyword::IFNDEF)) {
		lexer().next();
		do_ifndef();
		return true;
	}
	else
		return false;
}

bool Preproc::check_elif() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::ELIF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_elif();
		return true;
	}
	else if (l[0].is(Keyword::ELIF)) {
		lexer().next();
		do_elif();
		return true;
	}
	else
		return false;
}

bool Preproc::check_elifdef() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::ELIFDEF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_elifdef();
		return true;
	}
	else if (l[0].is(Keyword::ELIFDEF)) {
		lexer().next();
		do_elifdef();
		return true;
	}
	else
		return false;
}

bool Preproc::check_elifndef() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::ELIFNDEF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_elifndef();
		return true;
	}
	else if (l[0].is(Keyword::ELIFNDEF)) {
		lexer().next();
		do_elifndef();
		return true;
	}
	else
		return false;
}

bool Preproc::check_else() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::ELSE)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_else();
		return true;
	}
	else if (l[0].is(Keyword::ELSE)) {
		lexer().next();
		do_else();
		return true;
	}
	else
		return false;
}

bool Preproc::check_endif() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::ENDIF)) {
		if (ifs_active())
			do_label();
		else
			lexer().next();
		lexer().next();
		do_endif();
		return true;
	}
	else if (l[0].is(Keyword::ENDIF)) {
		lexer().next();
		do_endif();
		return true;
	}
	else
		return false;
}

bool Preproc::check_include() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Include)) {
		do_label();
		lexer().next();
		do_include();
		return true;
	}
	else if (l[0].is(Keyword::Include)) {
		lexer().next();
		do_include();
		return true;
	}
	else if (l[0].is(TType::Hash) && l[1].is(Keyword::Include)) {
		lexer().next();
		lexer().next();
		do_include();
		return true;
	}
	else
		return false;
}

bool Preproc::check_define() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Hash) && l[1].is(Keyword::Define)) {
		lexer().next();
		lexer().next();
		do_define();
		return true;
	}
	else
		return false;
}

bool Preproc::check_defcont() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Hash) && l[1].is(Keyword::Defcont)) {
		lexer().next();
		lexer().next();
		do_defcont();
		return true;
	}
	else
		return false;
}

bool Preproc::check_undef() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Hash) && l[1].is(Keyword::Undef)) {
		lexer().next();
		lexer().next();
		do_undef();
		return true;
	}
	else
		return false;
}

bool Preproc::check_defc_equ() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::DEFC)) {
		do_label();
		lexer().next();
		string name = expect_ident();
		expect_eq();
		do_defc(name);
		return true;
	}
	else if (l[0].is(Keyword::DEFC)) {
		lexer().next();
		string name = expect_ident();
		expect_eq();
		do_defc(name);
		return true;
	}
	else if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::EQU)) {
		string name = l[0].svalue;
		lexer().next();
		lexer().next();
		do_defc(name);
		return true;
	}
	else if (l[0].is(TType::Label, TType::Ident) && l[1].is(TType::Eq)) {
		string name = l[0].svalue;
		lexer().next();
		lexer().next();
		do_defc(name);
		return true;
	}
	else
		return false;
}

bool Preproc::check_defl() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::DEFL)) {
		string name = l[0].svalue;
		lexer().next();
		lexer().next();
		do_defl(name);
		return true;
	}
	else if (l[0].is(Keyword::DEFL)) {
		lexer().next();
		string name = expect_ident();
		expect_eq();
		do_defl(name);
		return true;
	}
	else
		return false;
}

bool Preproc::check_macro() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Macro)) {
		string name = l[0].svalue;
		lexer().next();
		lexer().next();
		do_macro(name);
		return true;
	}
	else if (l[0].is(Keyword::Macro)) {
		lexer().next();
		string name = expect_ident();
		do_macro(name);
		return true;
	}
	else if (l[0].is(Keyword::Endm)) 
		throw Error(ECode::StructNesting);
	else
		return false;
}

bool Preproc::check_local() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Local)) {
		do_label();
		lexer().next();
		do_local();
		return true;
	}
	else if (l[0].is(Keyword::Local)) {
		lexer().next();
		do_local();
		return true;
	}
	else
		return false;
}

bool Preproc::check_exitm() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Exitm)) {
		do_label();
		lexer().next();
		do_exitm();
		return true;
	}
	else if (l[0].is(Keyword::Exitm)) {
		lexer().next();
		do_exitm();
		return true;
	}
	else
		return false;
}

bool Preproc::check_rept() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Rept)) {
		do_label();
		lexer().next();
		do_rept();
		return true;
	}
	else if (l[0].is(Keyword::Rept)) {
		lexer().next();
		do_rept();
		return true;
	}
	else if (l[0].is(Keyword::Endr))
		throw Error(ECode::StructNesting);
	else
		return false;
}

bool Preproc::check_reptc() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Reptc)) {
		do_label();
		lexer().next();
		do_reptc();
		return true;
	}
	else if (l[0].is(Keyword::Reptc)) {
		lexer().next();
		do_reptc();
		return true;
	}
	else if (l[0].is(Keyword::Endr))
		throw Error(ECode::StructNesting);
	else
		return false;
}

bool Preproc::check_repti() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Repti)) {
		do_label();
		lexer().next();
		do_repti();
		return true;
	}
	else if (l[0].is(Keyword::Repti)) {
		lexer().next();
		do_repti();
		return true;
	}
	else if (l[0].is(Keyword::Endr))
		throw Error(ECode::StructNesting);
	else
		return false;
}

bool Preproc::check_defgroup() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Defgroup)) {
		do_label();
		lexer().next();
		do_defgroup();
		return true;
	}
	else if (l[0].is(Keyword::Defgroup)) {
		lexer().next();
		do_defgroup();
		return true;
	}
	else
		return false;
}

bool Preproc::check_defvars() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Defvars)) {
		do_label();
		lexer().next();
		do_defvars();
		return true;
	}
	else if (l[0].is(Keyword::Defvars)) {
		lexer().next();
		do_defvars();
		return true;
	}
	else
		return false;
}

bool Preproc::check_binary() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Binary, Keyword::IncBin)) {
		do_label();
		lexer().next();
		do_binary();
		return true;
	}
	else if (l[0].is(Keyword::Binary, Keyword::IncBin)) {
		lexer().next();
		do_binary();
		return true;
	}
	else
		return false;
}

bool Preproc::check_line() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Line)) {
		do_label();
		lexer().next();
		do_line(false);
		return true;
	}
	else if (l[0].is(Keyword::Line)) {
		lexer().next();
		do_line(false);
		return true;
	}
	else
		return false;
}

bool Preproc::check_c_line() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::CLine)) {
		do_label();
		lexer().next();
		do_line(true);
		return true;
	}
	else if (l[0].is(Keyword::CLine)) {
		lexer().next();
		do_line(true);
		return true;
	}
	else
		return false;
}

bool Preproc::check_macro_call() {
	Lexer& l{ lexer() };

	if (l[0].is(TType::Label, TType::Ident) && l[1].is(TType::Ident)) {
		string name = l[1].svalue;
		shared_ptr<Macro> macro = m_macros.find_all(name);
		if (macro) {
			do_label();
			lexer().next();
			do_macro_call(macro);
			return true;
		}
	}

	if (l[0].is(TType::Ident)) {
		string name = l[0].svalue;
		shared_ptr<Macro> macro = m_macros.find_all(name);
		if (macro) {
			lexer().next();
			do_macro_call(macro);
			return true;
		}
	}

	return false;
}

void Preproc::expect_eol() {
	if (!lexer().at_end())
		throw Error(ECode::EolExpected);
}

TType Preproc::expect_comma_or_rparens() {
	switch (lexer().peek().ttype) {
	case TType::Comma:
		lexer().next();
		return TType::Comma;
	case TType::Rparen:
		lexer().next();
		return TType::Rparen;
	default:
		throw Error(ECode::RparenOrCommaExpected);
	}
}

void Preproc::expect_eq() {
	if (lexer().peek().ttype != TType::Eq)
		throw Error(ECode::EqExpected);
	lexer().next();
}

void Preproc::expect_comma() {
	if (lexer().peek().ttype != TType::Comma)
		throw Error(ECode::CommaExpected);
	lexer().next();
}

string Preproc::expect_ident() {
	if (!lexer().peek().is(TType::Ident))
		throw Error(ECode::IdentExpected);

	string name = lexer().peek().svalue;
	lexer().next();
	return name;
}

bool Preproc::expect_if_flag() {
	Value value = expect_expr();
	bool flag = value.defined() && value.value() != 0;
	return flag;
}

bool Preproc::expect_idef_defined() {
	string name = expect_ident();
	expect_eol();
	if (defines().find_all(name))
		return true;		// #define exists
	else if (m_macros.find_all(name))
		return true;		// macro exists
	else if (m_symtab.find(name))
		return true;		// symbol exists
	else
		return false;
}

int Preproc::expect_const_expr(bool at_end) {
	Value value = expect_expr(at_end);
	if (!value.defined())
		throw Error(ECode::ConstExprExpected);
	return value.value();
}

Value Preproc::expect_expr(bool at_end) {
	// expand macros in expression
	size_t pos = lexer().pos();
	string text = string(lexer().text_ptr());
	ExpandedText expanded = expand(lexer(), defines());
	if (!expanded.got_error())
		text = expanded.text();

	// parse and evaluate expression in text
	Expr expr;
	Lexer sub_lexer{ text };
	expr.parse(sub_lexer);

	if (at_end) {
		if (!sub_lexer.at_end())
			throw Error(ECode::EolExpected);
	}
	else {
		size_t num_tokens = expr.tokens().size();
		lexer().set_pos(pos + num_tokens);		// advance past expression
	}
	Value value = expr.evaluate(m_symtab);
	return value;
}

string Preproc::expect_macro_body() {
	string body;

	while (getline()) {
		Lexer& l{ lexer() };

		if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Macro))
			throw Error(ECode::StructNesting);
		else if (l[0].is(Keyword::Macro))
			throw Error(ECode::StructNesting);
		else if (l[0].is(Keyword::Endm)) {
			lexer().next();
			expect_eol();
			return body;
		}
		else
			body += line().text + "\n";
	}
	throw Error(ECode::StructNesting);
}

string Preproc::expect_rept_body() {
	string body;

	while (getline()) {
		Lexer& l{ lexer() };

		if (l[0].is(TType::Label, TType::Ident) && l[1].is(Keyword::Rept, Keyword::Repti, Keyword::Reptc))
			throw Error(ECode::StructNesting);
		else if (l[0].is(Keyword::Rept, Keyword::Repti, Keyword::Reptc))
			throw Error(ECode::StructNesting);
		else if (l[0].is(Keyword::Endr)) {
			lexer().next();
			expect_eol();
			return body;
		}
		else
			body += line().text + "\n";
	}
	throw Error(ECode::StructNesting);
}

TType Preproc::next_token_getline() {
	while (true) {
		if (lexer().at_end()) {
			if (!getline())
				throw Error(ECode::StructNesting);
		}
		else
			return lexer().peek().ttype;
	}
}

int Preproc::expect_size_id() {
	switch (lexer().peek().keyword) {
	case Keyword::DS_B:
		lexer().next();
		return 1;
	case Keyword::DS_W:
		lexer().next();
		return 2;
	case Keyword::DS_P:
		lexer().next();
		return 3;
	case Keyword::DS_Q:
		lexer().next();
		return 4;
	default:
		return -1;
	}
}

string Preproc::expect_string() {
	if (!lexer().peek().is(TType::String))
		throw Error(ECode::StringExpected);

	string str = lexer().peek().svalue;
	lexer().next();
	return str;
}

void Preproc::do_label() {
    Line line1{ line() };
    line1.text = lexer().peek().svalue + ":";
    m_output.push_back(line1);
    lexer().next();
}

void Preproc::do_include() {
	string filename = expect_string();
	expect_eol();
    parse(filename);
}

void Preproc::do_define() {
	// get name
	size_t name_col = lexer().peek().col;
	string name = expect_ident();

	// check if name is followed by '(' without spaces
	size_t this_col = lexer().peek().col;
	bool has_space = (this_col > name_col + name.length());
	bool has_args = (!has_space && lexer().peek().is(TType::Lparen));

	// create macro
	auto macro = make_shared<Macro>(name);
	defines().add(macro);						// create macro
	m_last_macro = macro;

	// collect args
	if (has_args) {
		lexer().next();							// skip '('
		while (true) {
			string arg = expect_ident();
			macro->push_arg(arg);
			if (expect_comma_or_rparens() == TType::Comma)
				continue;						// ','
			else
				break;							// ')'
		}
	}

	// collect body
	const char* body_ptr = lexer().text_ptr();
	macro->push_body(body_ptr);

	while (!lexer().at_end())
		lexer().next();
}

void Preproc::do_defcont() {
	if (!m_last_macro)
		throw Error(ECode::StructNesting);

	// collect body
	const char* body_ptr = lexer().text_ptr();
	m_last_macro->push_body("\n");
	m_last_macro->push_body(body_ptr);

	while (!lexer().at_end())
		lexer().next();
}

void Preproc::do_undef() {
	string name = expect_ident();
	expect_eol();
	defines().remove(name);
}

void Preproc::do_defc(const string& name) {
	// expand macros in expression
	string text = string(lexer().text_ptr());
	ExpandedText expanded = expand(lexer(), defines());
	if (!expanded.got_error())
		text = expanded.text();

	// output DEFC line
	Line line1 = line();
	line1.text = "defc " + name + "=" + text;
	m_output.push_back(line1);

	// parse and evaluate expression in text
	Expr expr;
	Lexer lexer{ text };
	expr.parse(lexer);
	if (!lexer.at_end())
		throw Error(ECode::EolExpected);
	Value value = expr.evaluate(m_symtab);

	// store symbol
	auto symbol = make_shared<Symbol>(name);
	symbol->value() = value;
	m_symtab.add(symbol);
}

void Preproc::do_defl(const string& name) {
	// if name is not defined, create an empty one
	if (!defines_base().find(name)) {
		auto macro = make_shared<Macro>(name);
		defines_base().add(macro);
	}

	// expand macros in expression, may refer to name
	if (lexer().at_end())
		throw Error(ECode::ArgExpected);

	string text = string(lexer().text_ptr());
	ExpandedText expanded = expand(lexer(), defines());
	if (!expanded.got_error())
		text = expanded.text();

	// redefine name
	defines_base().remove(name);
	auto macro = make_shared<Macro>(name);
	macro->push_body(text);
	defines_base().add(macro);
}

void Preproc::do_if() {
	bool flag = expect_if_flag();
	m_ifs.emplace_back(Keyword::IF, flag);
	m_ifs.back().done_if = m_ifs.back().done_if || flag;
}

void Preproc::do_ifdef() {
	bool defined = expect_idef_defined();
	m_ifs.emplace_back(Keyword::IF, defined);
	m_ifs.back().done_if = m_ifs.back().done_if || defined;
}

void Preproc::do_ifndef() {
	bool defined = expect_idef_defined();
	m_ifs.emplace_back(Keyword::IF, !defined);
	m_ifs.back().done_if = m_ifs.back().done_if || !defined;
}

void Preproc::do_elif() {
	if (m_ifs.empty())
		throw Error(ECode::StructNesting);
	Keyword last = m_ifs.back().keyword;
	if (last != Keyword::IF && last != Keyword::ELIF)
		throw Error(ECode::StructNesting);

	bool flag = expect_if_flag();
	if (m_ifs.back().done_if)
		flag = false;
	m_ifs.back().keyword = Keyword::ELIF;
	m_ifs.back().flag = flag;
	m_ifs.back().done_if = m_ifs.back().done_if || flag;
}

void Preproc::do_elifdef() {
	if (m_ifs.empty())
		throw Error(ECode::StructNesting);
	Keyword last = m_ifs.back().keyword;
	if (last != Keyword::IF && last != Keyword::ELIF)
		throw Error(ECode::StructNesting);

	bool defined = expect_idef_defined();
	if (m_ifs.back().done_if)
		defined = false;
	m_ifs.back().keyword = Keyword::ELIF;
	m_ifs.back().flag = defined;
	m_ifs.back().done_if = m_ifs.back().done_if || defined;
}

void Preproc::do_elifndef() {
	if (m_ifs.empty())
		throw Error(ECode::StructNesting);
	Keyword last = m_ifs.back().keyword;
	if (last != Keyword::IF && last != Keyword::ELIF)
		throw Error(ECode::StructNesting);

	bool notdef = !expect_idef_defined();
	if (m_ifs.back().done_if)
		notdef = false;
	m_ifs.back().keyword = Keyword::ELIF;
	m_ifs.back().flag = notdef;
	m_ifs.back().done_if = m_ifs.back().done_if || notdef;
}

void Preproc::do_else() {
	expect_eol();
	if (m_ifs.empty())
		throw Error(ECode::StructNesting);
	Keyword last = m_ifs.back().keyword;
	if (last != Keyword::IF && last != Keyword::ELIF)
		throw Error(ECode::StructNesting);

	bool flag = !m_ifs.back().done_if;
	m_ifs.back().keyword = Keyword::ELSE;
	m_ifs.back().flag = flag;
	m_ifs.back().done_if = m_ifs.back().done_if || flag;
}

void Preproc::do_endif() {
	expect_eol();
	if (m_ifs.empty())
		throw Error(ECode::StructNesting);
	Keyword last = m_ifs.back().keyword;
	if (last != Keyword::IF && last != Keyword::ELIF && last != Keyword::ELSE)
		throw Error(ECode::StructNesting);
	m_ifs.pop_back();
}

void Preproc::do_macro(const string& name) {
	// create macro
	auto macro = make_shared<Macro>(name);
	m_macros.add(macro);								// create macro

	// collect args
	if (!lexer().at_end()) {
		while (true) {
			string arg = expect_ident();
			macro->push_arg(arg);
			if (lexer().peek().is(TType::Comma))
				lexer().next();						// skip comma and continue
			else
				break;
		}
	}
	expect_eol();

	string body = expect_macro_body();
	macro->push_body(body);
}

void Preproc::do_macro_call(shared_ptr<Macro> macro) {
	// collect arguments
	vector<string> params;
	if (macro->args().size() != 0) {
		params = collect_macro_params(lexer());
		if (macro->args().size() != params.size())
			Error::warning(cerr, ECode::WrongNumberMacroArgs, macro->name());
	}
	expect_eol();

	// create new level of macro expansion
	m_levels.emplace_back(&defines());

	// create macros for each argument
	for (size_t i = 0; i < macro->args().size(); i++) {
		string arg = macro->args()[i];
		string param = i < params.size() ? params[i] : "";
		shared_ptr<Macro> param_macro = make_shared<Macro>(arg, param);
		defines().add(param_macro);
	}

	// create lines from body
	Line line1 = line();
	line1.text = macro->body();
	m_levels.back().init(line1);
}

void Preproc::do_local() {
	// collect symbols
	while (true) {
		string name = expect_ident();
		string def_name = unique_name(name);
		auto macro = make_shared<Macro>(name);
		macro->push_body(def_name);
		defines().add(macro);

		if (lexer().peek().is(TType::Comma))
			lexer().next();						// skip comma and continue
		else
			break;
	}
	expect_eol();
}

void Preproc::do_exitm() {
	expect_eol();
	if (m_levels.size() > 1)
		m_levels.back().exitm_called = true;
	else
		throw Error(ECode::StructNesting);
}

void Preproc::do_rept() {
	int count = expect_const_expr();
	string body = expect_rept_body();

	// create new level for expansion
	m_levels.emplace_back(&defines());
	string block;
	for (int i = 0; i < count; i++)
		block += body;

	// create lines from body
	Line line1 = line();
	line1.text = block;
	m_levels.back().init(line1);
}

void Preproc::do_reptc() {
	string var = expect_ident();
	expect_comma();

	string str;
	Token token = lexer().peek();
	lexer().next();

	if (token.ttype == TType::String) 
		str = token.svalue;
	else if (token.ttype == TType::Integer)
		str = std::to_string(token.ivalue);
	else if (token.ttype == TType::Ident) {
		// expand macros in text
		ExpandedText out;
		expand_ident(out, token.svalue, lexer(), defines());

		Lexer sub_lexer{ out.text() };
		token = sub_lexer.peek();
		sub_lexer.next();

		if (token.ttype == TType::Integer) 
			str = std::to_string(token.ivalue);
		else if (token.ttype == TType::Ident) 
			str = token.svalue;
		else
			throw Error(ECode::StringExpected);
		if (!sub_lexer.at_end())
			throw Error(ECode::EolExpected);
	}
	else
		throw Error(ECode::StringExpected);
	expect_eol();

	string body = expect_rept_body();

	// create new level for expansion
	m_levels.emplace_back(&defines());
	string block = "\n";			// lines starting with # are not split
	for (auto& c : str) {
		block += "#undef " + var + "\n";
		block += "#define " + var + " " + std::to_string(c) + "\n";
		block += body;
	}

	// create lines from body
	Line line1 = line();
	line1.text = block;
	m_levels.back().init(line1);
}

void Preproc::do_repti() {
	string var = expect_ident();
	expect_comma();
	if (lexer().at_end())
		throw Error(ECode::ArgExpected);

	vector<string> params = collect_macro_params(lexer());
	expect_eol();

	string body = expect_rept_body();

	// expand macros in parameters
	for (auto& param : params) {
		Lexer sub_lexer{ param };
		ExpandedText expanded = expand(sub_lexer, defines());
		if (!expanded.got_error())
			param = expanded.text();
	}

	// create new level for expansion
	m_levels.emplace_back(&defines());
	string block = "\n";			// lines starting with # are not split
	for (auto& param : params) {
		block += "#undef " + var + "\n";
		block += "#define " + var + " " + param + "\n";
		block += body;
	}

	// create lines from body
	Line line1 = line();
	line1.text = block;
	m_levels.back().init(line1);
}

void Preproc::do_defgroup() {
	if (next_token_getline() != TType::Lbrace)
		throw Error(ECode::LbraceExpected);
	lexer().next();

	int value = 0;
	while (true) {
		next_token_getline();

		// get name[=expression]
		string name = expect_ident();
		if (lexer().peek().ttype == TType::Eq) {
			lexer().next();
			value = expect_const_expr(false);
		}

		// output defc name=expression
		Line line1 = line();
		line1.text = "defc " + name + "=" + std::to_string(value);
		m_output.push_back(line1);

		// store symbol
		auto symbol = make_shared<Symbol>(name);
		symbol->value() = Value(value);
		m_symtab.add(symbol);

		value++;

		// expect comma or rbrace
		TType ttype = next_token_getline();
		if (ttype == TType::Comma) {
			lexer().next();
			continue;
		}
		else if (ttype == TType::Rbrace) {
			lexer().next();
			break;
		}
		else
			throw Error(ECode::RbraceOrCommaExpected);
	}
	expect_eol();
}

void Preproc::do_defvars() {
	// get origin
	next_token_getline();
	int origin = expect_const_expr(false);

	// open brace
	if (next_token_getline() != TType::Lbrace)
		throw Error(ECode::LbraceExpected);
	lexer().next();

	int value = origin < 0 ? m_defvars_value : origin;
	while (true) {
		next_token_getline();

		// get [name] size_id count
		string name;
		int size = expect_size_id();
		if (size < 0) {
			name = expect_ident();
			size = expect_size_id();
			if (size < 0)
				throw Error(ECode::SizeIdExpected);
		}

		int count = expect_const_expr(false);

		if (!name.empty()) {
			// output defc name=expression
			Line line1 = line();
			line1.text = "defc " + name + "=" + std::to_string(value);
			m_output.push_back(line1);

			// store symbol
			auto symbol = make_shared<Symbol>(name);
			symbol->value() = Value(value);
			m_symtab.add(symbol);
		}

		value += size * count;
		if (origin != 0)
			m_defvars_value = value;

		// expect comma or rbrace
		TType ttype = next_token_getline();
		if (ttype == TType::Ident) 
			continue;
		else if (ttype == TType::Rbrace) {
			lexer().next();
			break;
		}
		else
			throw Error(ECode::RbraceOrIdentExpected);
	}
	expect_eol();
}

void Preproc::do_binary() {
	string filename = expect_string();
	expect_eol();

	// open binary file
	string filename_path{ search_file(filename, g_options.include_path) };
	if (filename_path.empty())
		throw Error(ECode::FileNotFound, filename);
	ifstream ifs(filename_path, ios::binary);
	if (!ifs.is_open())
		throw Error(ECode::OpenFile, filename_path);

	// output DEFB lines
	const int line_len = 64;
	unsigned char bytes[line_len];

	while (!ifs.eof()) {
		ifs.read(reinterpret_cast<char*>(bytes), line_len);
		size_t num_read = static_cast<size_t>(ifs.gcount());
		if (num_read > 0) {
			Line line1 = line();
			string separator = "";
			line1.text = "defb ";
			for (size_t i = 0; i < num_read; i++) {
				line1.text += separator + std::to_string(bytes[i]);
				separator = ",";
			}
			m_output.push_back(line1);
		}
	}
}

void Preproc::do_line(bool is_c_line) {
	int line_num = expect_const_expr(false);
	string filename;

	if (lexer().peek().ttype == TType::Comma) {
		lexer().next();
		filename = expect_string();
	}
	expect_eol();

	if (is_c_line) {
		// output LINE statement
		Line line1 = line();
		line1.text = "c_line " + std::to_string(line_num);
		if (!filename.empty())
			line1.text += ",\"" + filename + "\"";
		m_output.push_back(line1);
	}
	else {
		// modify current input location
		if (!is_c_line) {
			m_files.back().location.line_num = line_num - 1;
			if (!filename.empty())
				m_files.back().location.filename = filename;
		}
	}
}

ExpandedText Preproc::expand(Lexer& lexer, Macros& defines) {
	ExpandedText out;

	while (!lexer.at_end()) {
		Token token = lexer[0];
		lexer.next();

		switch (token.ttype) {
		case TType::End:
			break;
		case TType::Ident:
			expand_ident(out, token.svalue, lexer, defines);
			break;
		case TType::Label:
			expand_ident(out, token.svalue, lexer, defines);
			out.append(":");
			break;
		case TType::Newline: out.append("\n"); break;
		case TType::Integer: out.append(to_string(token.ivalue)); break;
		case TType::Floating: out.append(float_to_byte_list(token.fvalue)); break;
		case TType::String: out.append(str_to_byte_list(token.svalue)); break;
		case TType::LogNot: out.append("!"); break;
		case TType::Hash: out.append("#"); break;
		case TType::DblHash: out.append("##"); break;
		case TType::ASMPC: out.append("$"); break;
		case TType::Mod: out.append("%"); break;
		case TType::BinAnd: out.append("&"); break;
		case TType::LogAnd: out.append("&&"); break;
		case TType::Lparen: out.append("("); break;
		case TType::Rparen: out.append(")"); break;
		case TType::Mul: out.append("*"); break;
		case TType::Pow: out.append("**"); break;
		case TType::Plus: out.append("+"); break;
		case TType::Comma: out.append(","); break;
		case TType::Minus: out.append("-"); break;
		case TType::Dot: out.append("."); break;
		case TType::Div: out.append("/"); break;
		case TType::Colon: out.append(":"); break;
		case TType::Lt: out.append("<"); break;
		case TType::Le: out.append("<="); break;
		case TType::Shl: out.append("<<"); break;
		case TType::Eq: out.append("=="); break;
		case TType::Ne: out.append("!="); break;
		case TType::Gt: out.append(">"); break;
		case TType::Ge: out.append(">="); break;
		case TType::Shr: out.append(">>"); break;
		case TType::Quest: out.append("?"); break;
		case TType::Lsquare: out.append("["); break;
		case TType::Backslash: out.append("\n"); break;
		case TType::Rsquare: out.append("]"); break;
		case TType::BinXor: out.append("^"); break;
		case TType::LogXor: out.append("^^"); break;
		case TType::Lbrace: out.append("{"); break;
		case TType::BinOr: out.append("|"); break;
		case TType::LogOr: out.append("||"); break;
		case TType::Rbrace: out.append("}"); break;
		case TType::BinNot: out.append("~"); break;
		default: assert(0);
		}
	}

	return out;
}

void Preproc::expand_ident(ExpandedText& out, const string& ident, Lexer& lexer, Macros& defines) {
	size_t pos = lexer.pos();
	ExpandedText expanded = expand_define_call(ident, lexer, defines);
	if (expanded.got_error()) {
		lexer.set_pos(pos);
		out.append(ident);
	}
	else
		out.append(expanded.text());
}

ExpandedText Preproc::expand_define_call(const string& name, Lexer& lexer, Macros& defines) {
	ExpandedText out;

	shared_ptr<Macro> macro = defines.find_all(name);
	if (!macro) {							// macro does not exists - insert name
		out.append(name);
		return out;
	}

	// macro exists
	if (macro->is_expanding()) {				// recursive invocation
		out.append(macro->body());
		out.set_error(true);
		return out;
	}

	// collect arguments
	vector<string> params;
	if (macro->args().size() != 0) {
		params = collect_macro_params(lexer);
		if (macro->args().size() != params.size())
			Error::warning(cerr, ECode::WrongNumberMacroArgs, macro->name());		
	}

	// create macros for each argument
	Macros sub_defines{ defines };				// create scope for arguments
	for (size_t i = 0; i < macro->args().size(); i++) {
		string arg = macro->args()[i];
		string param = i < params.size() ? params[i] : "";
		shared_ptr<Macro> param_macro = make_shared<Macro>(arg, param);
		sub_defines.add(param_macro);
	}

	// expand macro
	macro->set_expanding(true);
	try {
		Lexer sub_lexer{ macro->body() };
		out = expand(sub_lexer, sub_defines);
	}
	catch (const Error& e) {
		cerr << e;				// show error and continue
		out.set_error(true);
	}
	macro->set_expanding(false);
	return out;
}

string Preproc::collect_param(Lexer& lexer) {
	const char* p0 = lexer.text_ptr();
	int open_parens = 0;
	while (true) {
		const char* p1 = lexer.text_ptr();
		switch (lexer.peek().ttype) {
		case TType::End:
			return string(p0, p1);
		case TType::Lparen:
			open_parens++;
			lexer.next();
			break;
		case TType::Rparen:
			open_parens--;
			if (open_parens < 0) 
				return string(p0, p1);
			else
				lexer.next();
			break;
		case TType::Comma:
			if (open_parens == 0) 
				return string(p0, p1);
			else
				lexer.next();
			break;
		default:
			lexer.next();
		}
	}
}

vector<string> Preproc::collect_macro_params(Lexer& lexer) {
	vector<string> params;

	bool in_parens = (lexer.peek().ttype == TType::Lparen);
	if (in_parens)
		lexer.next();

	// collect up to close parens or end of line
	while (true) {
		params.push_back(collect_param(lexer));
		switch (lexer.peek().ttype) {
		case TType::Comma:
			lexer.next();
			continue;
		case TType::Rparen:
			if (in_parens)
				lexer.next();
			return params;
		case TType::End:
			return params;
		default:
			throw Error(ECode::RparenOrCommaExpected);
		}
	}
}

bool Preproc::ifs_active() const {
	if (m_levels.back().exitm_called)
		return false;
	for (auto& f : m_ifs) {
		if (!f.flag)
			return false;
	}
	return true;
}

void preproc_file(const string& filename) {
    if (g_options.verbose)
        cout << "Processing file: " << filename << endl;
    Preproc pp{ filename };
    pp.parse();
    pp.write();
}

int next_id() {
	static int id = 0;
	return ++id;
}

string unique_name(const string& name) {
	return name + "__" + std::to_string(next_id());
}

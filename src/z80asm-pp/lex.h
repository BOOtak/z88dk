//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstring>
#include <string>
#include <vector>
using namespace std;

inline bool isident(char c) {
    return c == '_' || isalnum(c);
}

enum class TType {
    End, Newline,
    Ident, Label, Integer, Floating, String, ASMPC,
    BinNot, LogNot, BinAnd, LogAnd, BinOr, LogOr, BinXor, LogXor,
    Plus, Minus, Mul, Pow, Div, Mod,
    Eq, Ne, Lt, Le, Gt, Ge, Shl, Shr,
    Quest, Colon, Dot, Comma, Hash, DblHash,
    Lparen, Rparen, Lsquare, Rsquare, Lbrace, Rbrace,
    Backslash,
};

enum class Keyword {
#	define X(id, text)		id,
#	include "keyword.def"
};

struct Token {
    TType   ttype{ TType::End };
    int     ivalue{ 0 };
    double	fvalue{ 0.0 };
    string  svalue;
    Keyword	keyword{ Keyword::None };
    size_t	col{ 0 };

    Token(TType ttype = TType::End, int ivalue = 0)
        : ttype(ttype), ivalue(ivalue) {}
    Token(TType ttype, double fvalue)
        : ttype(ttype), fvalue(fvalue) {}
    Token(TType ttype, const string& svalue, Keyword keyword = Keyword::None)
        : ttype(ttype), svalue(svalue), keyword(keyword) {}

    void clear() { *this = Token(); }

    bool is(TType ttype) { return this->ttype == ttype; }
    bool is(Keyword keyword) { return this->keyword == keyword; }
    template <typename T, typename... Types>
    bool is(T var1, Types... var2) {
        if (is(var1))
            return true;
        else if (is(var2...))
            return true;
        else
            return false;
    }
};

class Lexer {
public:
    Lexer(const string& text = "") { set(text); }
    void set(const string& text = "");
    void clear() { set(); }

    const string& text() const { return m_text; }
	const char* text_ptr() const {
		return m_pos < m_tokens.size() ?
			m_text.c_str() + m_tokens[m_pos].col :
			m_text.c_str() + m_text.length();
	}
    size_t size() const { return m_tokens.size(); }

    void rewind() { m_pos = 0; }
    bool at_end() const { return m_pos >= m_tokens.size(); }
    size_t pos() const { return m_pos; }
    void set_pos(size_t pos) { m_pos = pos; }

    Token& peek(int offset = 0);			// 0: current; 1: next; -1: previous
    Token& operator[](int offset);
    void next();

private:
    string			m_text;
    vector<Token>	m_tokens;
    size_t			m_pos;

#if 0
    auto begin() { return m_tokens.begin(); }
    auto end() { return m_tokens.end(); }
    auto empty() const { return m_tokens.empty(); }
    auto front() { return m_tokens.front(); }
    void pop_front() { m_tokens.pop_front(); }
    void set(const string& text = "") { m_text = text; m_ptr = m_text.c_str(); }
    const char* get_pos() const { return m_ptr; }
    void set_pos(const char* p) { m_ptr = p; }
    void skip_spaces();
    void skip_end() { m_ptr += strlen(m_ptr); }
    TType peek();
    TType lex();
    char peek_char() const { return *m_ptr; }
    const Token& token() const { return m_token; }
#endif

private:
#if 0
    string		m_text;
    const char* m_ptr;
    Token		m_token;

    TType lex1();
#endif

};

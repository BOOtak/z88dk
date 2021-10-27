//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once
#include "lex.h"
#include "symtab.h"
#include <cassert>
#include <string>
#include <vector>
using namespace std;

enum class NodeType {
	LeafNumber, LeafIdent, LeafASMPC,
	Immediate, TernCond, LogOr, LogXor, LogAnd, BinOr, BinXor, BinAnd,
	Lt, Le, Gt, Ge, Eq, Ne, 
	Shl, Shr, Plus, Minus, Mul, Div, Mod, Pow,
	UnaryMinus, LogNot, BinNot,
};

class ExprNode {
	static const size_t MAX_ARGS = 3;
public:
	ExprNode(NodeType type = NodeType::LeafNumber, vector<ExprNode*> args = {});
	ExprNode(int n);
	ExprNode(const std::string& ident);
	ExprNode(const ExprNode& other);
	ExprNode(ExprNode&& other);
	ExprNode& operator=(const ExprNode& other);
	ExprNode& operator=(ExprNode&& other);
	~ExprNode();
	ExprNode* clone() const;

	NodeType type() const { return m_type; }
	Value& value() { return m_value; }
	const string& ident() const { return m_ident; };
	ExprNode* arg(size_t i) { assert(i < MAX_ARGS); return m_args[i]; }

private:
	NodeType	m_type{ NodeType::LeafNumber };
	Value		m_value;
	string		m_ident;
	ExprNode*	m_args[MAX_ARGS]{};
};

class Expr {
public:
	Expr() {};
	~Expr() { delete m_root; }
	void clear();

	vector<Token>& tokens() { return m_tokens; };
	ExprNode* root() const { return m_root; }
	void set_root(ExprNode* root) { delete m_root; m_root = root; }

	Value evaluate(Symtab& symtab);			// throws exceptions on errors

	// parse an expression
	void parse(Lexer& lexer);

private:
	vector<Token>	m_tokens;
	ExprNode*		m_root{ nullptr };
	Lexer*			m_lexer{ nullptr };		// used during parsing

	Value evaluate(Symtab& symtab, ExprNode* node);

	// parser
	TType ttype() const { return m_lexer->peek().ttype; }
	Token token() const { return m_lexer->peek(); }
	void advance();

	ExprNode* parse_expr();
	ExprNode* parse_immediate();
	ExprNode* parse_ternary_condition();
	ExprNode* parse_logical_or();
	ExprNode* parse_logical_and();
	ExprNode* parse_binary_or();
	ExprNode* parse_binary_and();
	ExprNode* parse_condition();
	ExprNode* parse_shift();
	ExprNode* parse_addition();
	ExprNode* parse_multiplication();
	ExprNode* parse_power();
	ExprNode* parse_unary();
	ExprNode* parse_primary();
};


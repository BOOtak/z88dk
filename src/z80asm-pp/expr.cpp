//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "expr.h"
#include "error.h"

static int ipow(int base, int exp) {
	int result = 1;
	for (;;) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}
	return result;
}

ExprNode::ExprNode(NodeType type, vector<ExprNode*> args)
	: m_type(type) {
	for (size_t i = 0; i < MAX_ARGS; i++) {
		if (i < args.size())
			m_args[i] = args[i];
		else
			m_args[i] = nullptr;
	}
}

ExprNode::ExprNode(int n)
	: m_type(NodeType::LeafNumber) {
	m_value.set_value(n);
}

ExprNode::ExprNode(const std::string& ident)
	: m_type(NodeType::LeafIdent), m_ident(ident) {
}

ExprNode::ExprNode(const ExprNode& other)
	: m_type(other.m_type), m_value(other.m_value), m_ident(other.m_ident) {
	for (size_t i = 0; i < MAX_ARGS; i++) {
		if (other.m_args[i])
			m_args[i] = other.m_args[i]->clone();
		else
			m_args[i] = nullptr;
	}
}

ExprNode::ExprNode(ExprNode&& other)
	: m_type(other.m_type), m_value(other.m_value), m_ident(other.m_ident) {
	for (size_t i = 0; i < MAX_ARGS; i++) {
		if (other.m_args[i])
			m_args[i] = other.m_args[i];
		else
			m_args[i] = nullptr;
	}
}

ExprNode& ExprNode::operator=(const ExprNode& other) {
	if (&other != this) {
		m_type = other.m_type;
		m_value = other.m_value;
		m_ident = other.m_ident;
		for (size_t i = 0; i < MAX_ARGS; i++) {
			delete m_args[i];
			if (other.m_args[i])
				m_args[i] = other.m_args[i]->clone();
			else
				m_args[i] = nullptr;
		}
	}
	return *this;
}

ExprNode& ExprNode::operator=(ExprNode&& other) {
	if (&other != this) {
		m_type = other.m_type;
		m_value = other.m_value;
		m_ident = other.m_ident;
		for (size_t i = 0; i < MAX_ARGS; i++) {
			delete m_args[i];
			if (other.m_args[i])
				m_args[i] = other.m_args[i];
			else
				m_args[i] = nullptr;
		}
	}
	return *this;
}

ExprNode::~ExprNode() {
	for (size_t i = 0; i < MAX_ARGS; i++)
		delete m_args[i];
}

ExprNode* ExprNode::clone() const {
	ExprNode* node = new ExprNode();
	node->m_type = m_type;
	node->m_value = m_value;
	node->m_ident = m_ident;
	for (size_t i = 0; i < MAX_ARGS; i++) {
		if (m_args[i])
			node->m_args[i] = m_args[i]->clone();
		else
			node->m_args[i] = nullptr;
	}
	return node;
}

void Expr::clear() {
	m_tokens.clear();
	delete m_root;
	m_root = nullptr;
}

Value Expr::evaluate(Symtab& symtab) {
	return evaluate(symtab, m_root);
}

Value Expr::evaluate(Symtab& symtab, ExprNode* node) {
	shared_ptr<Symbol> symbol;

	Value undefined;
	undefined.set_defined(false);

	assert(node);

	// functions without arguments
	switch (node->type()) {
	case NodeType::LeafNumber:
		return node->value();
	case NodeType::LeafIdent:
		symbol = symtab.find(node->ident());
		if (symbol)
			return symbol->value();
		else
			return undefined;
	case NodeType::LeafASMPC:
		return undefined;
	default:;
	}

	// functions with one argument and ternary condition
	Value a = evaluate(symtab, node->arg(0));
	if (!a.defined())
		return a;
	switch (node->type()) {
	case NodeType::TernCond:
		if (a.value())
			return evaluate(symtab, node->arg(1));
		else
			return evaluate(symtab, node->arg(2));
	case NodeType::Immediate:
		return a;
	case NodeType::UnaryMinus:
		return Value(-a.value());
	case NodeType::LogNot:
		return Value(!a.value());
	case NodeType::BinNot:
		return Value(~a.value());
	default:;
	}

	// functions with two arguments
	Value b = evaluate(symtab, node->arg(1));
	if (!b.defined())
		return b;
	switch (node->type()) {
	case NodeType::LogOr:
		return Value(a.value() || b.value());
	case NodeType::LogXor:
		return Value(a.value() != b.value());
	case NodeType::LogAnd:
		return Value(a.value() && b.value());
	case NodeType::BinOr:
		return Value(a.value() | b.value());
	case NodeType::BinXor:
		return Value(a.value() ^ b.value());
	case NodeType::BinAnd:
		return Value(a.value() & b.value());
	case NodeType::Lt:
		return Value(a.value() < b.value());
	case NodeType::Le:
		return Value(a.value() <= b.value());
	case NodeType::Gt:
		return Value(a.value() > b.value());
	case NodeType::Ge:
		return Value(a.value() >= b.value());
	case NodeType::Eq:
		return Value(a.value() == b.value());
	case NodeType::Ne:
		return Value(a.value() != b.value());
	case NodeType::Shl:
		return Value(a.value() << b.value());
	case NodeType::Shr:
		return Value(a.value() >> b.value());
	case NodeType::Plus:
		return Value(a.value() + b.value());
	case NodeType::Minus:
		return Value(a.value() - b.value());
	case NodeType::Mul:
		return Value(a.value() * b.value());
	case NodeType::Div:
		if (b.value() == 0)
			throw Error(ECode::DivisionByZero);
		return Value(a.value() / b.value());
	case NodeType::Mod:
		if (b.value() == 0)
			throw Error(ECode::DivisionByZero);
		return Value(a.value() % b.value());
	case NodeType::Pow:
		return Value(ipow(a.value(), b.value()));
	default:;
	}

	assert(0); return Value();
}

void Expr::parse(Lexer& lexer) {
	m_lexer = &lexer;
	clear();
	m_root = parse_expr();
}

void Expr::advance() {
	m_tokens.push_back(m_lexer->peek());
	m_lexer->next();
}

ExprNode* Expr::parse_expr() {
	return parse_immediate();
}

ExprNode* Expr::parse_immediate() {
	if (ttype() == TType::Hash) {
		advance();
		ExprNode* node = new ExprNode(NodeType::Immediate,
			{ parse_immediate() });
		return node;
	}
	else
		return parse_ternary_condition();
}

ExprNode* Expr::parse_ternary_condition() {
	ExprNode* node = parse_logical_or();
	if (ttype() == TType::Quest) {
		advance();
		ExprNode* t = parse_ternary_condition();
		if (ttype() != TType::Colon)
			throw Error(ECode::ColonExpected);
		ExprNode* f = parse_ternary_condition();
		node = new ExprNode(NodeType::TernCond, { node, t, f });
	}
	return node;
}

ExprNode* Expr::parse_logical_or() {
	ExprNode* node = parse_logical_and();
	while (true) {
		switch (ttype()) {
		case TType::LogOr:
			advance();
			node = new ExprNode(NodeType::LogOr, { node, parse_logical_and() });
			break;
		case TType::LogXor:
			advance();
			node = new ExprNode(NodeType::LogXor, { node, parse_logical_and() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_logical_and() {
	ExprNode* node = parse_binary_or();
	while (true) {
		switch (ttype()) {
		case TType::LogAnd:
			advance();
			node = new ExprNode(NodeType::LogAnd, { node, parse_binary_or() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_binary_or() {
	ExprNode* node = parse_binary_and();
	while (true) {
		switch (ttype()) {
		case TType::BinOr:
			advance();
			node = new ExprNode(NodeType::BinOr, { node, parse_binary_and() });
			break;
		case TType::BinXor:
			advance();
			node = new ExprNode(NodeType::BinXor, { node, parse_binary_and() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_binary_and() {
	ExprNode* node = parse_condition();
	while (true) {
		switch (ttype()) {
		case TType::BinAnd:
			advance();
			node = new ExprNode(NodeType::BinAnd, { node, parse_condition() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_condition() {
	ExprNode* node = parse_shift();
	while (true) {
		switch (ttype()) {
		case TType::Lt:
			advance();
			node = new ExprNode(NodeType::Lt, { node, parse_shift() });
			break;
		case TType::Le:
			advance();
			node = new ExprNode(NodeType::Le, { node, parse_shift() });
			break;
		case TType::Gt:
			advance();
			node = new ExprNode(NodeType::Gt, { node, parse_shift() });
			break;
		case TType::Ge:
			advance();
			node = new ExprNode(NodeType::Ge, { node, parse_shift() });
			break;
		case TType::Eq:
			advance();
			node = new ExprNode(NodeType::Eq, { node, parse_shift() });
			break;
		case TType::Ne:
			advance();
			node = new ExprNode(NodeType::Ne, { node, parse_shift() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_shift() {
	ExprNode* node = parse_addition();
	while (true) {
		switch (ttype()) {
		case TType::Shl:
			advance();
			node = new ExprNode(NodeType::Shl, { node, parse_addition() });
			break;
		case TType::Shr:
			advance();
			node = new ExprNode(NodeType::Shr, { node, parse_addition() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_addition() {
	ExprNode* node = parse_multiplication();
	while (true) {
		switch (ttype()) {
		case TType::Plus:
			advance();
			node = new ExprNode(NodeType::Plus,
				{ node, parse_multiplication() });
			break;
		case TType::Minus:
			advance();
			node = new ExprNode(NodeType::Minus,
				{ node, parse_multiplication() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_multiplication() {
	ExprNode* node = parse_power();
	while (true) {
		switch (ttype()) {
		case TType::Mul:
			advance();
			node = new ExprNode(NodeType::Mul, { node, parse_power() });
			break;
		case TType::Div:
			advance();
			node = new ExprNode(NodeType::Div, { node, parse_power() });
			break;
		case TType::Mod:
			advance();
			node = new ExprNode(NodeType::Mod, { node, parse_power() });
			break;
		default:
			return node;
		}
	}
}

ExprNode* Expr::parse_power() {
	ExprNode* node = parse_unary();
	if (ttype() == TType::Pow) {
		advance();
		node = new ExprNode(NodeType::Pow, { node, parse_power() });
	}
	return node;
}

ExprNode* Expr::parse_unary() {
	ExprNode* node{ nullptr };
	switch (ttype()) {
	case TType::Minus:
		advance();
		return new ExprNode(NodeType::UnaryMinus, { parse_unary() });
	case TType::Plus:
		advance();
		return parse_unary();
	case TType::LogNot:
		advance();
		return new ExprNode(NodeType::LogNot, { parse_unary() });
	case TType::BinNot:
		advance();
		return new ExprNode(NodeType::BinNot, { parse_unary() });
	case TType::Lparen:
		advance();
		node = parse_expr();
		if (ttype() != TType::Rparen)
			throw Error(ECode::UnbalancedParens);
		advance();
		return node;
	case TType::Lsquare:
		advance();
		node = parse_expr();
		if (ttype() != TType::Rsquare)
			throw Error(ECode::UnbalancedParens);
		advance();
		return node;
	default:
		return parse_primary();
	}
}

ExprNode* Expr::parse_primary() {
	ExprNode* node;
	switch (ttype()) {
	case TType::Ident:
		switch (token().keyword) {
		case Keyword::ASMPC:
			advance();
			return new ExprNode(NodeType::LeafASMPC);
		default:
			node = new ExprNode(token().svalue);
			advance();
			return node;
		}
	case TType::Integer:
		node = new ExprNode(token().ivalue);
		advance();
		return node;
	default:
		throw Error(ECode::IntegerOrSymbolExpected);
	}
}

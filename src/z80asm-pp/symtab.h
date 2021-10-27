//-----------------------------------------------------------------------------
// z80asm preprocessor
// Copyright (C) Paulo Custodio, 2011-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Macro {
public:
    Macro(const string& name, const string& body = "");

    const string& name() const { return m_name; }
    const vector<string>& args() const { return m_args; }
    const string& body() const { return m_body; }
	bool is_expanding() const { return m_is_expanding; }
	void set_expanding(bool f) { m_is_expanding = f; }

    void push_arg(const string& arg);
    void push_body(const string& text);

private:
    string			m_name;
    vector<string>	m_args;
    string			m_body;
	bool				m_is_expanding{ false };
};

class Macros {
public:
    Macros(Macros* parent = nullptr);

    void add(shared_ptr<Macro> macro);
	void remove(const string& name);

    shared_ptr<Macro> find(const string& name) const;
    shared_ptr<Macro> find_all(const string& name) const;

	auto begin() { return m_table.begin(); }
	auto end() { return m_table.end(); }

private:
    Macros* m_parent;
    unordered_map<string, shared_ptr<Macro>> m_table;
};

class Value {
public:
	explicit Value(int value = 0) : m_defined(true), m_value(value) {}
	void clear() { m_defined = true; m_value = 0; }

	bool defined() const { return m_defined; }
	void set_defined(bool f) { m_defined = f; }

	int value() const { return m_value; }
	void set_value(int value) { m_value = value; m_defined = true; }

private:
	bool		m_defined{ true };
	int		m_value{ 0 };
};

class Symbol {
public:
	Symbol(const string& name) : m_name(name) {}

	const string& name() const { return m_name; }
	Value& value() { return m_value; }

private:
	string	m_name;
	Value	m_value;
};

class Symtab {
public:
	void add(shared_ptr<Symbol> symbol);
	void remove(const string& name);

	shared_ptr<Symbol> find(const string& name) const;

private:
	unordered_map<string, shared_ptr<Symbol>> m_table;
};

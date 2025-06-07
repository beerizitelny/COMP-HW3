#ifndef COMP_HW3_SYMBOLTABLE_HPP
#define COMP_HW3_SYMBOLTABLE_HPP

#include <deque>
#include <stack>
#include <string>
#include <vector>
#include "nodes.hpp"
#include "output.hpp"

/*
 * This files takes care of the Symbol Tables uses in the HW.
 * Each line (entry) of the ST represents a variable in the code
 * and has it's name, type and offset.
 * Note: each scope has it's own ST.
 */

/*****************************************************************************************
                                    SYMBOL TABLE ENTRY
******************************************************************************************/


class SymTableEntry {
    std::string name;
    ast::BuiltInType type;
    bool is_func_decl;
    unsigned int offset;
    std::vector<ast::BuiltInType> *param_types;
    std::vector<std::string> *string_param_types;

public:
    SymTableEntry(const std::string &name, ast::BuiltInType type, bool is_func_decl = false, unsigned int offset = 0) :
        name(name), type(type), is_func_decl(is_func_decl), offset(offset),
        param_types(new std::vector<ast::BuiltInType>()), string_param_types(new std::vector<std::string>()) {};

    const std::string get_name() const { return name; }
    ast::BuiltInType get_type() const { return type; }
    bool is_func_decl() const { return is_func_decl; }
    unsigned int get_offset() const { return offset; }
    std::vector<ast::BuiltInType> *const get_param_types() const { return param_types; }
    std::vector<std::string> *const get_string_param_types() const { return string_param_types; }

    void add_param(ast::BuiltInType type);
};

/*****************************************************************************************
                                    SYMBOL TABLE
******************************************************************************************/

class SymTable {
    std::vector<SymTableEntry *> *entries;

public:
    SymTable() : entries(new std::vector<SymTableEntry *>()) {};
    ~SymTable();
    /* append a new entry in the table */
    void add_new_entry(SymTableEntry *entry);
    SymTableEntry *get_entry_by_id(const std::string &id);
};

/*****************************************************************************************
                                   SYMBOL TABLE STACK
******************************************************************************************/


class SymTableStack {
    std::deque<SymTable *> *sym_tables;
    std::stack<unsigned int> *offsets;
    std::stack<SymTableEntry *> *function_scopes;

public:
    SymTableStack() :
        sym_tables(new std::deque<SymTable *>), offsets(new std::stack<unsigned int>),
        function_scopes(new std::stack<SymTableEntry *>()) {

        SymTable *new_symbol_table = new SymTable();
        sym_tables->push_back(new_symbol_table);
        offsets->push(0);
    };
    ~SymTableStack();

    output::ScopePrinter scope_printer;

    /*
     * pushes a new table to the stack.
     * use in case of a new scope.
     */
    void push_table();

    /*
     * pops the top table from the stack.
     * use at the end of scope.
     */
    void pop_table();

    /*
     * pushes a new entry to the top table.
     * use in case of a new declaration.
     */
    void push_entry(SymTableEntry *entry);

    /*
     * pushes a new entry to the top table.
     * use in case of a new declaration.
     */
    SymTableEntry *get_symbol_entry_by_id(const std::string &id);

    SymTableEntry *get_current_function_scope() const { return function_scopes->top(); }

    void delete_current_function_scope() { function_scopes->pop(); }
};

#endif // COMP_HW3_SYMBOLTABLE_HPP

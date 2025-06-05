#ifndef COMP_HW3_SYMBOLTABLE_HPP
#define COMP_HW3_SYMBOLTABLE_HPP

#include "nodes.hpp"
#include "output.hpp"
#include <vector>
#include <stack>
#include <deque>
#include <string>

/*
 * This files takes care of the Symbol Tables uses in the HW.
 * Each line (entry) of the ST represents a variable in the code
 * and has it's name, type and offset.
 * Note: each scope has it's own ST.
 */

class SymTableEntry {
    std::string name;
    ast::BuiltInType type;
    bool is_func_decl;
    unsigned int offset;
    std::vector<ast::BuiltInType>* param_types;

public:
    SymTableEntry(const std::string& name, ast::BuiltInType type, bool is_func_decl = false, unsigned int offset = 0) :
        name(name),
        type(type),
        is_func_decl(is_func_decl),
        offset(offset),
        param_types(new std::vector<ast::BuiltInType>()) {};

    unsigned int get_offset() const {return offset;}
};

class SymTable {
    std::vector<SymTableEntry*>* entries;

public:
    SymTable() : entries(new std::vector<SymTableEntry*>()) {};
    ~SymTable();
    /* append a new entry in the table */
    void add_new_entry(SymTableEntry* entry);
};

class SymTableStack {
    std::deque<SymTable*>* sym_tables;
    std::stack<unsigned int>* offsets;
    output::ScopePrinter scope_printer;

public:
    SymTableStack() : sym_tables(new std::deque<SymTable*>), offsets(new std::stack<unsigned int>) {
        sym_tables->push(new SymTable());
        offsets->push(0);
    };
    ~SymTableStack();

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
    void push_entry(SymTableEntry* entry);
};

#endif //COMP_HW3_SYMBOLTABLE_HPP

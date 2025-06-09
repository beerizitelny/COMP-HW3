#include "SymbolTable.hpp"
#include <algorithm>
#include <iostream>

void SymTableEntry::add_param(ast::BuiltInType type) {
    param_types->push_back(type);
    string_param_types->push_back(output::toString(type));
}

SymTable::~SymTable() {
    if (entries) {
        // Delete each SymTableEntry* in the vector
        for (auto entry: *entries) {
            delete entry;
        }
        // Delete the vector itself
        delete entries;
    }
}

SymTableEntry *SymTable::get_entry_by_id(const std::string &id) {
    auto entry_iterator = std::find_if(entries->begin(), entries->end(),
                                       [&id](SymTableEntry *entry) { return entry->get_name() == id; });

    if (entry_iterator == entries->end())
        return nullptr;

    return *entry_iterator;
}

void SymTable::add_new_entry(SymTableEntry *entry) { entries->push_back(entry); }

SymTableStack::~SymTableStack() {
    while (!sym_tables->empty()) {
        SymTable *sym_table = sym_tables->front();
        delete sym_table;
        sym_tables->pop_front();
    }
    delete sym_tables;
    delete offsets;
}

void SymTableStack::push_table() {
    // std::cout << " ~~ PUSHING" << std::endl;
    sym_tables->push_front(new SymTable);
    offsets->push(offsets->top());
}

void SymTableStack::pop_table() {
    // std::cout << " ## POPPING" << std::endl;
    if (!sym_tables->empty() && !offsets->empty()) {
        delete sym_tables->front();
        sym_tables->pop_front();
        offsets->pop();
    }
}

void SymTableStack::push_entry(SymTableEntry *entry, int var_size) {
    sym_tables->front()->add_new_entry(entry);
    unsigned int offset = 0;
    if (!offsets->empty()) {
        offset = offsets->top();
        offsets->pop();
    }
    offset += var_size;
    offsets->push(offset);
    // print_entries();
}

SymTableEntry *SymTableStack::get_symbol_entry_by_id(const std::string &id) {
    for (auto sym_table: *sym_tables) {
        SymTableEntry *entry = sym_table->get_entry_by_id(id);
        if (entry)
            return entry;
    }
    return nullptr;
}

#include "SymbolTable.hpp"

SymTable::~SymTable() {
    if (entries) {
        // Delete each SymTableEntry* in the vector
        for (auto entry : *entries) {
            delete entry;
        }
        // Delete the vector itself
        delete entries;
    }
}

void SymTable::add_new_entry(SymTableEntry *entry) {
    entries->push_back(entry);
}

SymTableStack::~SymTableStack() {
    while (!sym_tables->empty()){
        SymTable* sym_table = sym_tables->front();
        delete sym_table;
        sym_tables->pop_front();
    }
    delete sym_tables;
    delete offsets;
}

void SymTableStack::push_table() {
    sym_tables->push(new SymTable);
    offsets->push(offsets->top());
}

void SymTableStack::pop_table() {
    if (!sym_tables->empty() && !offsets->empty()){
        delete sym_tables->front();
        sym_tables->pop_front();
        offsets->pop();
    }
}

void SymTableStack::push_entry(SymTableEntry *entry) {
    sym_tables->front()->add_new_entry(entry);
    unsigned int offset = 0;
    if (!offsets->empty()){
        offset = offsets->top();
        offsets->pop();
    }
    offset += entry->get_offset();
    offsets->push(offset);
}

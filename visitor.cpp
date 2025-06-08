
#include "SymbolTable.hpp"
#include "nodes.hpp"

// TODO: remove this later! and remove prints
#include <iostream>

#include <cassert>
using namespace std;

class SemanticParserVisitor : Visitor {
protected:
    SymTableStack symbol_table_stack;
    SymTable declared_functions;

    bool looping = false;

    bool is_valid_cast(ast::BuiltInType from, ast::BuiltInType to) {
        return ((from == to) || (from == ast::BuiltInType::BYTE && to == ast::BuiltInType::INT));
    }

public:
    /** constructor */
    SemanticParserVisitor() : symbol_table_stack(), declared_functions() {}

    /***************************************************************************************
                            implementation of helper methods
    ****************************************************************************************/
    int calc_bin_op (ast::BinOpType op, int var1, int var2) {
        switch (op) {
            case ast::BinOpType::ADD:
                return var1 + var2;
            case ast::BinOpType::DIV:
                return var1 / var2;
            case ast::BinOpType::MUL:
                return var1 * var2;
            case ast::BinOpType::SUB:
                return var1 - var2;
        }
    }

    SymTableEntry * validate_array_dereference(string id,std::shared_ptr<ast::Exp> index, int line){
        // checking if this is a valid id
        SymTableEntry *sym_entry = symbol_table_stack.get_symbol_entry_by_id(id);
        if (sym_entry == nullptr){
            output::errorUndef(line, id);
        }

        index->accept(*this);
        // checking that this is an array and that the index is valid
        if (!(index->is_number() && sym_entry->is_array)){
            output::errorMismatch(line);
        }

        return sym_entry;
    }

    /***************************************************************************************
                            implementation of visitor methods
    ****************************************************************************************/
    void visit(ast::Num &node) {
        cout << "visited NUM node" << endl;
        node.type == ast::BuiltInType::INT;
        node.numerical_value = node.value;
    }

    void visit(ast::NumB &node) {
        cout << "visited NUM_B node" << endl;
        if (node.value > 255)
            output::errorByteTooLarge(node.line, node.value);
        node.type == ast::BuiltInType::BYTE;
        node.numerical_value = node.value;
    }

    void visit(ast::String &node) {
        cout << "visited STRING node" << endl;
        node.type == ast::BuiltInType::STRING;
    }

    void visit(ast::Bool &node) {
        cout << "visited BOOL node" << endl;
        node.type == ast::BuiltInType::BOOL;
    }

    void visit(ast::ID &node) {
        cout << "visited ID node" << endl;

        std::string id = node.value;
        SymTableEntry *func_entry = declared_functions.get_entry_by_id(id);
        SymTableEntry *var_entry = symbol_table_stack.get_symbol_entry_by_id(id);
        if (func_entry)
            node.type = func_entry->get_type();
        else if (var_entry)
            node.type = var_entry->get_type();
        else
            output::errorUndef(node.line, id);
    }

    void visit(ast::BinOp &node) {
        cout << "visited BINOP node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that both left and right side types are INT/BYTE to allow binary operation, and set type as the type
        // with broader range.
        if (node.left->type == ast::BuiltInType::BYTE && node.right->type == ast::BuiltInType::BYTE)
            node.type = ast::BuiltInType::BYTE;

        else if ((node.left->type == ast::BuiltInType::INT || node.left->type == ast::BuiltInType::BYTE) &&
                 (node.right->type == ast::BuiltInType::INT || node.right->type == ast::BuiltInType::BYTE))
            node.type = ast::BuiltInType::INT;

        else
            output::errorMismatch(node.line);

        node.numerical_value = calc_bin_op(node.op, node.left->get_numerical_value(), node.right->get_numerical_value());
    }

    void visit(ast::RelOp &node) {
        cout << "visited RELOP node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that both left and right side types are INT/BYTE to allow relative operation.
        if ((node.left->type == ast::BuiltInType::INT || node.left->type == ast::BuiltInType::BYTE) &&
            (node.right->type == ast::BuiltInType::INT || node.right->type == ast::BuiltInType::BYTE))
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::Not &node) {
        cout << "visited NOT node" << endl;
        node.exp->accept(*this);

        // validate that the expression is of boolean type to allow negating operation
        if (node.exp->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::And &node) {
        cout << "visited AND node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that left and right sides are of boolean type to allow and operation
        if (node.left->type == ast::BuiltInType::BOOL && node.right->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::Or &node) {
        cout << "visited OR node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that left and right sides are of boolean type to allow or operation
        if (node.left->type == ast::BuiltInType::BOOL && node.right->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    // void visit(ast::Type &node) { };

    void visit(ast::ArrayType &node) {
        cout << "visited ArrayType node" << endl;
        assert(node.length != nullptr);
        // finding out the length of the array
        node.length->accept(*this);
    }

    void visit(ast::PrimitiveType &node) {
        cout << "visited PRIMITIVE_TYPE node" << endl;

        // nothing to do here
        // TODO: fill this up
    }

    void visit(ast::ArrayDereference &node) {
        cout << "visited ARRAY_DEREFERENCE node" << endl;

        node.id->accept(*this);
        //getting the sym_entry only if valid params
        SymTableEntry *sym_entry = validate_array_dereference(node.id->value, node.index, node.line);

        // updating the type
        node.type = sym_entry->get_type();
    }

    void visit(ast::ArrayAssign &node) {
        cout << "visited ARRAY_ASSIGN node" << endl;

        node.id->accept(*this);
        //getting the sym_entry only if valid params
        SymTableEntry *sym_entry = validate_array_dereference(node.id->value, node.index, node.line);
        node.exp->accept(*this);
        if(sym_entry->get_type() != node.exp->type){
            output::ErrorInvalidAssignArray(node.line, node.id->value);
        }
    }

    void visit(ast::Cast &node) {
        cout << "visited CAST node" << endl;
        node.exp->accept(*this);
        node.target_type->accept(*this);

        // validate that expression and type are of INT/BYTE types to allow casting
        if ((node.exp->type == ast::BuiltInType::INT || node.exp->type == ast::BuiltInType::BYTE) &&
            (node.target_type->type == ast::BuiltInType::INT || node.target_type->type == ast::BuiltInType::BYTE))
            node.type = node.target_type->type;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::ExpList &node) {
        cout << "visited EXP_LIST node" << endl;

        // visit each expression in the exps vector
        for (auto exp: node.exps) {
            exp->accept(*this);
        }
    }

    void visit(ast::Call &node) {
        cout << "visited CALL node" << endl;

        node.func_id->accept(*this);
        std::string id = node.func_id->value;
        SymTableEntry *func_entry = declared_functions.get_entry_by_id(id);
        SymTableEntry *var_entry = symbol_table_stack.get_symbol_entry_by_id(id);

        // skipping the case where both are defined since it is checked in func_decl and assign.
        // case 1 - function is declared somewhere
        if (func_entry) {
            node.args->accept(*this);

            // validate number of parameters match before iterating to validate parameter typre
            if (func_entry->get_param_types()->size() != node.args->exps.size())
                output::errorPrototypeMismatch(node.line, id, *func_entry->get_string_param_types());

            for (int i = 0; i < func_entry->get_param_types()->size(); ++i) {
                if (!is_valid_cast(node.args->exps[i]->type, (*func_entry->get_param_types())[i]))
                    output::errorPrototypeMismatch(node.line, id, *func_entry->get_string_param_types());
            }
        }

        // case 2 - function isn't declared, but a variable bearing this id was declared
        else if (var_entry)
            output::errorDefAsVar(node.line, id);

        // case 3 - function isn't declared, no variable with this id exists
        else
            output::errorMismatch(node.line);
    }

    void visit(ast::Statements &node) {
        cout << "visited STATEMENTS node" << endl;

        // entry procedure - create new scope and symbol table for it
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();

        // visit all statements inside the scope
        for (auto statement: node.statements)
            statement->accept(*this);

        // entry procedure - close new scope and remove its symbol table
        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::Break &node) {
        cout << "visited BREAK node" << endl;

        if (!looping)
            output::errorUnexpectedBreak(node.line);
    };

    void visit(ast::Continue &node) {
        cout << "visited CONTINUE node" << endl;

        if (!looping)
            output::errorUnexpectedContinue(node.line);
    }

    void visit(ast::Return &node) {
        cout << "visited RETURN node" << endl;

        SymTableEntry *current_function_scope = symbol_table_stack.get_current_function_scope();
        if (node.exp)
            node.exp->accept(*this);

        // validate that if expression type matches function return type, or function returns void and no expression is
        // used
        if ((node.exp && !is_valid_cast(node.exp->type, current_function_scope->get_type())) ||
            (!node.exp && current_function_scope->get_type() != ast::BuiltInType::VOID))
            output::errorMismatch(node.line);
    }

    void visit(ast::If &node) {
        cout << "visited IF node" << endl;

        // if statements always create a new scope
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();
        node.condition->accept(*this);
        if (node.condition->type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        // this might create a new scope inside the if scope
        node.then->accept(*this);

        // deleting the if scope
        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();

        // else statements always create a new scope - if else attribute is set
        if (node.otherwise) {
            symbol_table_stack.push_table();
            symbol_table_stack.scope_printer.beginScope();
            node.otherwise->accept(*this);
            symbol_table_stack.pop_table();
            symbol_table_stack.scope_printer.endScope();
        }
    }

    void visit(ast::While &node) {
        cout << "visited WHILE node" << endl;

        // while statements always create a new scope
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();
        looping = true;
        node.condition->accept(*this);
        if (node.condition->type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);
        node.body->accept(*this);
        looping = false;
        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::VarDecl &node) {
        cout << "visited VAR_DECL node" << endl;

        std::string id = node.id->value;
        if (declared_functions.get_entry_by_id(id) || symbol_table_stack.get_symbol_entry_by_id(id))
            output::errorDef(node.line, id);

        node.type->accept(*this);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            if (!is_valid_cast(node.init_exp->type, node.type->type))
                output::errorMismatch(node.line);
        }
        // calculate the offset whether it's an arrayType or primitiveType
        int offset = node.type->get_offset();
        SymTableEntry *new_symbol_table_entry = new SymTableEntry(id, node.type->type, offset);
        if (offset > 0) {
            new_symbol_table_entry->is_array = true;
        }

        symbol_table_stack.push_entry(new_symbol_table_entry);

        // visiting id only here to avoid premature lookup in symbol table
        node.id->accept(*this);
        // TODO: scope printer
    }

    void visit(ast::Assign &node) {
        cout << "visited ASSIGN node" << endl;

        // validation of prior declaration of this ID is done when visiting node.id
        node.id->accept(*this);
        std::string id = node.id->value;

        // cannot assign into a funcion identifier
        if (declared_functions.get_entry_by_id(id))
            output::errorDefAsFunc(node.line, id);

        node.exp->accept(*this);

        SymTableEntry *var_entry = symbol_table_stack.get_symbol_entry_by_id(id);

        // make sure that a variable bearing this id exists
        if (!var_entry)
            output::errorUndef(node.line, id);

        // validate matching types between expression and symbol as declared
        if (!is_valid_cast(node.exp->type, var_entry->get_type()))
            output::errorMismatch(node.line);
    }

    void visit(ast::Formal &node) {
        cout << "visited FORMAL node" << endl;

        node.id->accept(*this);
        node.type->accept(*this);
    }

    void visit(ast::Formals &node) {
        cout << "visited FORMALS node" << endl;

        int argument_offset = -1;
        for (auto formal: node.formals) {

            std::string formal_id = formal->id->value;
            if (declared_functions.get_entry_by_id(formal_id) || symbol_table_stack.get_symbol_entry_by_id(formal_id))
                output::errorDef(node.line, formal_id);

            SymTableEntry *new_symbol_table_entry = new SymTableEntry(formal_id, formal->type->type, argument_offset--);
            symbol_table_stack.push_entry(new_symbol_table_entry);
        }
        for (auto formal: node.formals) {
            formal->accept(*this);
        }
    }

    void visit(ast::FuncDecl &node) {
        cout << "visited FUNC_DECL node" << endl;

        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();

        // TODO: retrieve entry from the data-structure that holds function declarations (no need to validate existence
        // - Funcs ensures that)

        // TODO: insert entry into symbol table stack

        node.id->accept(*this);
        node.return_type->accept(*this);
        node.formals->accept(*this);
        for (auto statement: node.body->statements)
            statement->accept(*this);

        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::Funcs &node) {
        cout << "visited FUNCS node" << endl;

        // TODO: handle main func here - make sure it is defined and correctly implemented to comply with exercise
        // requirements

        // create symbol table entries for print and printi functions
        SymTableEntry *symbol_table_entry_print = new SymTableEntry("print", ast::BuiltInType::VOID, true);
        SymTableEntry *symbol_table_entry_printi = new SymTableEntry("printi", ast::BuiltInType::VOID, true);

        // TODO: add these function declarations to the data structure that holds function declarations

        // call scopePrinter's function emitter
        symbol_table_entry_print->add_param(ast::BuiltInType::STRING);
        symbol_table_entry_printi->add_param(ast::BuiltInType::INT);
        symbol_table_stack.scope_printer.emitFunc(symbol_table_entry_print->get_name(),
                                                  symbol_table_entry_print->get_type(),
                                                  *symbol_table_entry_print->get_param_types());
        symbol_table_stack.scope_printer.emitFunc(symbol_table_entry_printi->get_name(),
                                                  symbol_table_entry_printi->get_type(),
                                                  *symbol_table_entry_printi->get_param_types());

        for (auto func: node.funcs) {
            string func_id = func->id->value;
            ast::BuiltInType func_type = func->return_type->type;

            // TODO: make sure this function id is defined in the data-structure that holds function declarations

            SymTableEntry *new_entry = new SymTableEntry(func_id, func_type, true);
            for (auto func_param: func->formals->formals)
                new_entry->add_param(func_param->type->type); // TODO: handle PrimitiveType and ArrayType

            // TODO: add this function declaration to the data-stucture that holds function declarations
        }

        // visiting functions only here to avoid premature lookup in symbol table
        for (auto func: node.funcs)
            func->accept(*this);
    }
};

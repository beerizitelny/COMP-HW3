
#include <iostream>
#include "SymbolTable.hpp"
#include "nodes.hpp"

static void print_node_name(const std::string &name) {
    if (false)
        std::cout << "visited " << name << " node" << std::endl;
}

class SemanticParserVisitor : public Visitor {
protected:
    // SymTable declared_functions;

    unsigned int whiles = 0;
    bool looping() { return whiles > 0; }
    bool is_valid_cast(ast::BuiltInType from, ast::BuiltInType to) {
        return ((from == to) || (from == ast::BuiltInType::BYTE && to == ast::BuiltInType::INT));
    }

public:
    SymTableStack symbol_table_stack;

    /** constructor */
    SemanticParserVisitor() : symbol_table_stack() {}

    /***************************************************************************************
                            implementation of helper methods
    ****************************************************************************************/
    int calc_bin_op(ast::BinOpType op, int var1, int var2) {
        switch (op) {
            case ast::BinOpType::ADD:
                return var1 + var2;
            case ast::BinOpType::DIV:
                return var1 / var2;
            case ast::BinOpType::MUL:
                return var1 * var2;
            case ast::BinOpType::SUB:
                return var1 - var2;
            default:
                return -1;
        }
    }

    SymTableEntry *validate_array_dereference(std::string id, std::shared_ptr<ast::Exp> index, int line) {
        // checking if this is a valid id
        SymTableEntry *sym_entry = symbol_table_stack.get_symbol_entry_by_id(id);
        if (sym_entry == nullptr) {
            output::errorUndef(line, id);
        }
        if (sym_entry->is_func_decl) {
            output::errorDefAsFunc(line, id);
        }
        index->accept(*this);
        // checking that this is an array and that the index is valid
        if (!(index->is_number() && sym_entry->is_array)) {
            output::errorMismatch(line);
        }

        return sym_entry;
    }

    /***************************************************************************************
                            implementation of visitor methods
    ****************************************************************************************/
    void visit(ast::Num &node) {
        print_node_name("NUM");
        node.type = ast::BuiltInType::INT;
        node.numerical_value = node.value;
    }

    void visit(ast::NumB &node) {
        print_node_name("NUM_B");
        if (node.value > 255)
            output::errorByteTooLarge(node.line, node.value);
        node.type = ast::BuiltInType::BYTE;
        node.numerical_value = node.value;
    }

    void visit(ast::String &node) {
        print_node_name("STRING");
        node.type = ast::BuiltInType::STRING;
    }

    void visit(ast::Bool &node) {
        print_node_name("BOOL");
        node.type = ast::BuiltInType::BOOL;
    }

    void visit(ast::ID &node) {
        print_node_name("ID");

        std::string id = node.value;
        SymTableEntry *entry = symbol_table_stack.get_symbol_entry_by_id(id);
        if (!entry)
            output::errorUndef(node.line, id);

        node.type = entry->get_type();
    }

    void visit(ast::BinOp &node) {
        print_node_name("BINOP");
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

        node.numerical_value =
                calc_bin_op(node.op, node.left->get_numerical_value(), node.right->get_numerical_value());
    }

    void visit(ast::RelOp &node) {
        print_node_name("RELOP");
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
        print_node_name("NOT");
        node.exp->accept(*this);

        // validate that the expression is of boolean type to allow negating operation
        if (node.exp->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::And &node) {
        print_node_name("AND");
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that left and right sides are of boolean type to allow and operation
        if (node.left->type == ast::BuiltInType::BOOL && node.right->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::Or &node) {
        print_node_name("OR");
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
        print_node_name("ArrayType");
        // finding out the length of the array
        node.length->accept(*this);
    }

    void visit(ast::PrimitiveType &node) { print_node_name("PRIMITIVE_TYPE"); }

    void visit(ast::ArrayDereference &node) {
        print_node_name("ARRAY_DEREFERENCE");

        node.id->accept(*this);
        // getting the sym_entry only if valid params
        SymTableEntry *sym_entry = validate_array_dereference(node.id->value, node.index, node.line);

        // updating the type
        node.type = sym_entry->get_type();
    }

    void visit(ast::ArrayAssign &node) {
        print_node_name("ARRAY_ASSIGN");

        node.id->accept(*this);
        node.exp->accept(*this);

        // in case RHS is ID
        if (dynamic_cast<ast::ID *>(node.exp.get())) {
            // in case RHS is an array
            std::string rhs_id = ((ast::ID *) node.exp.get())->value;
            if (symbol_table_stack.get_symbol_entry_by_id(rhs_id)->is_array)
                output::errorMismatch(node.line);
            // in case RHS is a function
            if (symbol_table_stack.get_symbol_entry_by_id(rhs_id)->is_func_decl)
                output::errorDefAsFunc(node.line, rhs_id);
        }

        // getting the sym_entry only if valid params
        SymTableEntry *sym_entry = validate_array_dereference(node.id->value, node.index, node.line);

        // check if types match and that rvalue is not of array type
        if (sym_entry->get_type() != node.exp->type ||
            (dynamic_cast<ast::ID *>(node.exp.get()) &&
             symbol_table_stack.get_symbol_entry_by_id(((ast::ID *) node.exp.get())->value)->is_array)) {
            output::errorMismatch(node.line);
        }
    }

    void visit(ast::Cast &node) {
        print_node_name("CAST");
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
        print_node_name("EXP_LIST");

        // visit each expression in the exps vector
        for (auto exp: node.exps) {
            exp->accept(*this);
        }
    }

    void visit(ast::Call &node) {
        print_node_name("CALL");

        std::string id = node.func_id->value;
        SymTableEntry *entry = symbol_table_stack.get_symbol_entry_by_id(id);

        if (!entry)
            output::errorUndefFunc(node.line, id);

        node.func_id->accept(*this);
        // skipping the case where both are defined since it is checked in func_decl and assign.

        // case 1 - function isn't declared, no variable with this id exists
        if (!entry)
            output::errorMismatch(node.line);

        // case 2 - function isn't declared, but a variable bearing this id was declared
        else if (!entry->is_func_decl)
            output::errorDefAsVar(node.line, id);

        // case 3 - function is declared somewhere
        else {
            node.args->accept(*this);

            // validate number of parameters match before iterating to validate parameter typre
            if (entry->get_param_types()->size() != node.args->exps.size())
                output::errorPrototypeMismatch(node.line, id, *entry->get_string_param_types());

            for (int i = 0; i < entry->get_param_types()->size(); ++i) {

                if (dynamic_cast<ast::ID *>(node.args->exps[i].get())) {
                    std::string arg_name = static_cast<ast::ID *>(node.args->exps[i].get())->value;
                    if (symbol_table_stack.get_symbol_entry_by_id(arg_name)->is_array)
                        output::errorPrototypeMismatch(node.line, id, *entry->get_string_param_types());
                }
                if (!is_valid_cast(node.args->exps[i]->type, (*entry->get_param_types())[i]))
                    output::errorPrototypeMismatch(node.line, id, *entry->get_string_param_types());
            }
            node.type = entry->get_type();
        }
    }

    void visit(ast::Statements &node) {
        print_node_name("STATEMENTS");

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
        print_node_name("BREAK");

        if (!looping())
            output::errorUnexpectedBreak(node.line);
    };

    void visit(ast::Continue &node) {
        print_node_name("CONTINUE");

        if (!looping())
            output::errorUnexpectedContinue(node.line);
    }

    void visit(ast::Return &node) {
        print_node_name("RETURN");

        ast::BuiltInType expected_return_type = symbol_table_stack.get_current_function_scope();

        if (node.exp)
            node.exp->accept(*this);

        // validate that if expression type matches function return type, or function returns void and no expression
        // is used
        if ((node.exp && !is_valid_cast(node.exp->type, expected_return_type)) ||
            (!node.exp && expected_return_type != ast::BuiltInType::VOID))
            output::errorMismatch(node.line);

        if (dynamic_cast<ast::ID *>(node.exp.get())) {
            // in case RHS is an array
            std::string exp_id = ((ast::ID *) node.exp.get())->value;
            if (symbol_table_stack.get_symbol_entry_by_id(exp_id)->is_array) {
                output::errorMismatch(node.line);
            }
            if (symbol_table_stack.get_symbol_entry_by_id(exp_id)->is_func_decl) {
                output::errorDefAsFunc(node.line, exp_id);
            }
        }
    }

    void visit(ast::If &node) {
        print_node_name("IF");

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
        print_node_name("WHILE");

        // while statements always create a new scope
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();
        whiles++;
        node.condition->accept(*this);
        if (node.condition->type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);
        node.body->accept(*this);
        whiles--;
        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::VarDecl &node) {
        print_node_name("VAR_DECL");

        std::string id = node.id->value;
        if (symbol_table_stack.get_symbol_entry_by_id(id))
            output::errorDef(node.line, id);

        node.type->accept(*this);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            if (dynamic_cast<ast::ID *>(node.init_exp.get())) {
                std::string rhs_id = ((ast::ID *) node.init_exp.get())->value;
                // if rvalue is of array type
                if (symbol_table_stack.get_symbol_entry_by_id(rhs_id)->is_array)
                    output::errorMismatch(node.line);
                // if rvalue is a function identifier
                if (symbol_table_stack.get_symbol_entry_by_id(((ast::ID *) node.init_exp.get())->value)->is_func_decl)
                    output::errorDefAsFunc(node.line, rhs_id);
            }

            if (!is_valid_cast(node.init_exp->type, node.type->type))
                output::errorMismatch(node.line);
        }
        // calculate the offset whether it's an arrayType or primitiveType
        int var_size = node.type->get_size();
        int offset = symbol_table_stack.get_next_offset();
        SymTableEntry *new_symbol_table_entry = new SymTableEntry(id, node.type->type, false, offset);
        symbol_table_stack.push_entry(new_symbol_table_entry, var_size);

        if (node.type->is_array) {
            new_symbol_table_entry->is_array = true;
            symbol_table_stack.scope_printer.emitArr(id, node.type->type, var_size, offset);
        }

        else
            symbol_table_stack.scope_printer.emitVar(id, node.type->type, offset);


        // visiting id only here to avoid premature lookup in symbol table
        node.id->accept(*this);
    }

    void visit(ast::Assign &node) {
        print_node_name("ASSIGN");

        // validation of prior declaration of this ID is done when visiting node.id
        node.id->accept(*this);
        std::string id = node.id->value;
        node.exp->accept(*this);
        SymTableEntry *var_entry = symbol_table_stack.get_symbol_entry_by_id(id);

        // in case RHS is ID
        if (dynamic_cast<ast::ID *>(node.exp.get())) {
            // in case RHS is an array
            std::string rhs_id = ((ast::ID *) node.exp.get())->value;
            if (symbol_table_stack.get_symbol_entry_by_id(rhs_id)->is_array)
                output::errorMismatch(node.line);
            // in case RHS is a function
            if (symbol_table_stack.get_symbol_entry_by_id(rhs_id)->is_func_decl)
                output::errorDefAsFunc(node.line, rhs_id);
        }

        // make sure that a variable bearing this id exists
        if (!var_entry)
            output::errorUndef(node.line, id);

        // cannot assign into an array
        if (var_entry->is_array) {
            output::ErrorInvalidAssignArray(node.line, node.id->value);
        }

        // cannot assign into a function identifier
        if (var_entry->is_func_decl)
            output::errorDefAsFunc(node.line, id);

        // validate matching types between expression and symbol as declared
        if (!is_valid_cast(node.exp->type, var_entry->get_type()))
            output::errorMismatch(node.line);
    }

    void visit(ast::Formal &node) {
        print_node_name("FORMAL");

        node.id->accept(*this);
        node.type->accept(*this);
    }

    void visit(ast::Formals &node) {
        print_node_name("FORMALS");

        int argument_offset = 0;
        for (auto formal: node.formals) {
            std::string formal_id = formal->id->value;
            if (symbol_table_stack.get_symbol_entry_by_id(formal_id))
                output::errorDef(node.line, formal_id);

            SymTableEntry *new_symbol_table_entry =
                    new SymTableEntry(formal_id, formal->type->type, false, --argument_offset);
            symbol_table_stack.push_entry(new_symbol_table_entry, 0);
            symbol_table_stack.scope_printer.emitVar(formal_id, formal->type->type, argument_offset);
        }
        for (auto formal: node.formals) {
            formal->accept(*this);
        }
    }

    void visit(ast::FuncDecl &node) {
        print_node_name("FUNC_DECL");
        symbol_table_stack.push_function_scope(node.return_type->type);
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();

        node.id->accept(*this);
        node.return_type->accept(*this);
        node.formals->accept(*this);
        for (auto statement: node.body->statements)
            statement->accept(*this);

        symbol_table_stack.pop_table();
        symbol_table_stack.pop_function_scope();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::Funcs &node) {
        print_node_name("FUNCS");

        // create symbol table entries for print and printi functions
        SymTableEntry *symbol_table_entry_print = new SymTableEntry("print", ast::BuiltInType::VOID, true);
        SymTableEntry *symbol_table_entry_printi = new SymTableEntry("printi", ast::BuiltInType::VOID, true);

        // call scopePrinter's function emitter
        symbol_table_entry_print->add_param(ast::BuiltInType::STRING);
        symbol_table_entry_printi->add_param(ast::BuiltInType::INT);

        symbol_table_stack.push_entry(symbol_table_entry_print, 0);
        symbol_table_stack.push_entry(symbol_table_entry_printi, 0);

        // emitting functions for printing
        symbol_table_stack.scope_printer.emitFunc(symbol_table_entry_print->get_name(),
                                                  symbol_table_entry_print->get_type(),
                                                  *symbol_table_entry_print->get_param_types());
        symbol_table_stack.scope_printer.emitFunc(symbol_table_entry_printi->get_name(),
                                                  symbol_table_entry_printi->get_type(),
                                                  *symbol_table_entry_printi->get_param_types());

        for (auto func: node.funcs) {
            std::string func_id = func->id->value;
            ast::BuiltInType func_type = func->return_type->type;

            if (symbol_table_stack.get_symbol_entry_by_id(func_id))
                output::errorDef(func->line, func_id);

            SymTableEntry *new_entry = new SymTableEntry(func_id, func_type, true);
            symbol_table_stack.push_entry(new_entry, 0);
            for (auto func_param: func->formals->formals)
                new_entry->add_param(func_param->type->type);

            symbol_table_stack.scope_printer.emitFunc(func_id, func_type, *new_entry->get_param_types());
        }

        // handling main function and visiting function declaration nodes
        bool found_main = false;
        for (auto func: node.funcs) {
            // visiting functions only here to avoid premature lookup in symbol table
            func->accept(*this);
            if (!func->id->value.compare("main")) {

                if (func->return_type->type != ast::BuiltInType::VOID || func->formals->formals.size())
                    output::errorMainMissing();

                found_main = true;
            }
        }
        if (!found_main)
            output::errorMainMissing();
    }
};

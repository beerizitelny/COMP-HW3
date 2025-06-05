
#include "nodes.hpp"
#include "SymbolTable.hpp"

// TODO: remove this later! and remove prints
#include <iostream>
using namespace std;

class SemanticParserVisitor: Visitor {
protected:

    bool looping = False;

    bool is_valid_cast(ast::BuiltInType from, ast::BuiltInType to) {
        return ((from == to) || (from == ast::BuiltInType::BYTE && to == ast::BuiltInType::INT));
    }

public:

    SymTableStack symbol_table_stack;

    /** constructor */
    SemanticParserVisitor() : symbol_table_stack() {}

    /***************************************************************************************
                            implementation of visitor methods
    ****************************************************************************************/
    void visit(ast::Num &node) {
        cout << "visited NUM node" << endl;
        node.type == ast::BuiltInType::INT
    }

    void visit(ast::NumB &node) {
        cout << "visited NUM_B node" << endl;
        if (node.value > 255)
            output::errorByteTooLarge(node.line);
        node.type == ast::BuiltInType::BYTE
    }

    void visit(ast::String &node) {
        cout << "visited STRING node" << endl;
        node.type == ast::BuiltInType::STRING;
    }

    void visit(ast::Bool &node) {
        cout << "visited BOOL node" << endl;
        node.type == ast::BuiltInType::BOOL
    }

    void visit(ast::ID &node) {
        cout << "visited ID node" << endl;

        // TODO: check function declarations if ID is a name of a function, and if so - set type to FUNC

        // TODO: if not a func, verify that the ID is defined in some previous scope in symbol table, and set the type as exists in symbol table entry. otherwise throw errorUndef
    }

    void visit(ast::BinOp &node) {
        cout << "visited BINOP node" << endl;
        node->left.accept(*this);
        node->right.accept(*this);

        // validate that both left and right side types are INT/BYTE to allow binary operation, and set type as the type with broader range.
        if (node.left->type == ast::BuiltInType::BYTE && node.right->type == ast::BuiltInType::BYTE)
            node.type = ast::BuiltInType::BYTE;

        else if ((node.left->type == ast::BuiltInType::INT || node.left->type == ast::BuiltInType::BYTE) &&
            (node.right->type == ast::BuiltInType::INT || node.right->type == ast::BuiltInType::BYTE))
            node.type = ast::BuiltInType:INT;

        else
            output::errorMismatch(node.line);
    }

    void visit(ast::RelOp &node) {
        cout << "visited RELOP node" << endl;
        node->left.accept(*this);
        node->right.accept(*this);

        // validate that both left and right side types are INT/BYTE to allow relative operation.
        if ((node.left->type == ast::BuiltInType::INT || node.left->type == ast::BuiltInType::BYTE) &&
            (node.right->type == ast::BuiltInType::INT || node.right->type == ast::BuiltInType::BYTE))
            node.type = ast::BuiltInType:BOOL;

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
        cout << "visited OR node" << endl;

        // TODO: fill this up
    }

    void visit(ast::PrimitiveType &node) {
        cout << "visited PRIMITIVE_TYPE node" << endl;

        // TODO: fill this up
    }

    void visit(ast::ArrayDereference &node) {
        cout << "visited ARRAY_DEREFERENCE node" << endl;

        // TODO: fill this up
    }

    void visit(ast::ArrayAssign &node) {
        cout << "visited ARRAY_ASSIGN node" << endl;

        // TODO: fill this up
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
        for (auto exp& : node.exps) {
            exp->accept(*this);
        }
    }

    void visit(ast::Call &node) {
        cout << "visited CALL node" << endl;

        // TODO: implement this
    }

    void visit(ast::Statements &node) {
        cout << "visited STATEMENTS node" << endl;

        // entry procedure - create new scope and symbol table for it
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();

        // visit all statements inside the scope
        for (auto statement& : node.statements)
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

        // TODO: retrieve the function at the top of the stack

        // TODO: if node.exp exists: visit, get type, and check if cast is valid between function return type and node type, throw mismatch if not

        // TODO: if it doesn't exist, check that function return type is VOID, throw mismatch if not
    }

    void visit(ast::If &node) {
        cout << "visited IF node" << endl;

        // if statements always create a new scope
        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();
        node.condition->accept(*this);
        if (node.condition->type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line)
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

        // TODO: make sure node.id doesn't exist in any of the previous scopes of variables or functions (no shadowing allowed)

        node.type->accept(*this);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            if (!is_valid_cast(node.init_exp->type, node.type->type))
                output::errorMismatch(node.line);
        }
        SymTableEntry* new_symbol_table_entry = new SymTableEntry(node.id->value, node.type->type);
        symbol_table_stack.push_entry(new_symbol_table_entry);

        // visiting id only here to avoid premature lookup in symbol table
        node.id->accept(*this);

    }

    void visit(ast::Assign &node) {
        cout << "visited ASSIGN node" << endl;
        // validation of prior declaration of this ID is done when visiting node.id
        node.id->accept(*this);

        // TODO: make sure node.id doesn't exist in any of the previous scopes of functions, otherwise throw errorDefAsFunc

        node.exp->accept(*this);

        // TODO: make sure cast from node.exp->type to sym_table_entry.type where it was defined, otherwise throw errorMismatch
    }

    void visit(ast::Formal &node) {
        cout << "visited FORMAL node" << endl;

        node.id->accept(*this);
        node.type->accept(*this);
    }

    void visit(ast::Formals &node) {
        cout << "visited FORMALS node" << endl;

        int argument_offset = -1;
        for (auto formal& : node.formals) {

            // TODO: make sure formal->id->value doesn't exist in some previous scope, otherwise throw errorDef

            SymTableEntry* new_symbol_table_entry = new SymTableEntry(node.id->value, node.type->type, argument_offset--);
            symbol_table_stack.push_entry(new_symbol_table_entry);
        }
        for (auto formal& : node.formals) {
            formal->accept(*this);
        }
    }

    void visit(ast::FuncDecl &node) {
        cout << "visited FUNC_DECL node" << endl;

        symbol_table_stack.push_table();
        symbol_table_stack.scope_printer.beginScope();

        // TODO: retrieve entry from the data-structure that holds function declarations (no need to validate existence - Funcs ensures that)

        // TODO: insert entry into symbol table stack

        node.id->accept(*this);
        node.return_type->accept(*this);
        node.formals->accept(*this);
        for (auto statement& : node.body->statement)
            statement->accept(*this);

        symbol_table_stack.pop_table();
        symbol_table_stack.scope_printer.endScope();
    }

    void visit(ast::Funcs &node) {
        cout << "visited FUNCS node" << endl;

        // TODO: handle main func here - make sure it is defined and correctly implemented to comply with exercise requirements

        // create symbol table entries for print and printi functions
        SymTableEntry* symbol_table_entry_print = new SymTableEntry("print", ast::BuiltInType::VOID, true);
        SymTableEntry* symbol_table_entry_printi = new SymTableEntry("printi", ast::BuiltInType::VOID, true);

        // TODO: add these function declarations to the data structure that holds function declarations

        // call scopePrinter's function emitter
        symbol_table_entry_print->param_types->push_back(ast::BuiltInType::STRING);
        symbol_table_entry_printi->param_types->push_back(ast::BuiltInType::INT);
        symbol_table_stack.scope_printer.emitFunc(
            symbol_table_entry_print->name,
            symbol_table_entry_print->type,
            string_param_types_vector
        );
        symbol_table_stack.scope_printer.emitFunc(
            symbol_table_entry_printi->name,
            symbol_table_entry_printi->type,
            int_param_types_vector
        );

        for (auto func& : node.funcs) {
            string func_id = func->id->value;
            ast::BuiltInType funct_type = new func->return_type->type;

            // TODO: make sure this function id is defined in the data-structure that holds function declarations

            SymTableEntry* new_entry = new SymTableEntry(func_id, func_type, true);
            for (auto func_param : func->formals->formals)
                new_entry->param_types->push_back(func_param->type->type); // TODO: handle PrimitiveType and ArrayType

            // TODO: add this function declaration to the data-stucture that holds function declarations
        }

        // visiting functions only here to avoid premature lookup in symbol table
        for (auto func& : node.funcs)
            func->accept(*this);
    }
}
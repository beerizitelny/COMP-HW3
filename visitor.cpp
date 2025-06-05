
#include "nodes.hpp"
#include "SymbolTable.hpp"

// TODO: remove this later! and remove prints
#include <iostream>
using namespace std;

class SemanticParserVisitor: Visitor {
protected:

    bool is_valid_cast(ast::BuiltInType from, ast::BuiltInType to) {
        return ((from == to) || (from == ast::BuiltInType::BYTE && to == ast::BuiltInType::INT));
    }

public:
    SymTableStack symbol_table_stack;

    // implementation of visit methods
    void visit(ast::Num &node) {
        cout << "visited NUM node" << endl;
        node.type == ast::BuiltInType::INT
    };

    void visit(ast::NumB &node) {
        cout << "visited NUM_B node" << endl;
        node.type == ast::BuiltInType::BYTE
    };

    void visit(ast::String &node) {
        cout << "visited STRING node" << endl;
        node.type == ast::BuiltInType::STRING;
    };

    void visit(ast::Bool &node) {
        cout << "visited BOOL node" << endl;
        node.type == ast::BuiltInType::BOOL
    };};

    void visit(ast::ID &node) {
        cout << "visited ID node" << endl;

        // TODO: check function declarations if ID is a name of a function, and if so - set type to FUNC

        // TODO: if not a func, verify that the ID is defined in some previous scope in symbol table, and set the type as exists in symbol table entry
    };

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
    };

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
    };

    void visit(ast::Not &node) {
        cout << "visited NOT node" << endl;
        node.exp->accept(*this);

        // validate that the expression is of boolean type to allow negating operation
        if (node.exp->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    };

    void visit(ast::And &node) {
        cout << "visited AND node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that left and right sides are of boolean type to allow and operation
        if (node.left->type == ast::BuiltInType::BOOL && node.right->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    };

    void visit(ast::Or &node) {
        cout << "visited OR node" << endl;
        node.left->accept(*this);
        node.right->accept(*this);

        // validate that left and right sides are of boolean type to allow or operation
        if (node.left->type == ast::BuiltInType::BOOL && node.right->type == ast::BuiltInType::BOOL)
            node.type = ast::BuiltInType::BOOL;

        else
            output::errorMismatch(node.line);
    };

    // void visit(ast::Type &node) { };

    void visit(ast::ArrayType &node) {
        cout << "visited OR node" << endl;

        // TODO: fill this up
    };

    void visit(ast::PrimitiveType &node) {
        cout << "visited PRIMITIVE_TYPE node" << endl;

        // TODO: fill this up
    };

    void visit(ast::ArrayDereference &node) {
        cout << "visited ARRAY_DEREFERENCE node" << endl;

        // TODO: fill this up
    };

    void visit(ast::ArrayAssign &node) {
        cout << "visited ARRAY_ASSIGN node" << endl;

        // TODO: fill this up
    };

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
    };

    void visit(ast::ExpList &node) {
        cout << "visited EXP_LIST node" << endl;

        // visit each expression in the exps vector
        for (auto exp& : node.exps) {
            exp->accept(*this);
        }
    };

    void visit(ast::Call &node) {
        cout << "visited CALL node" << endl;

        // TODO: implement this
    };

    void visit(ast::Statements &node) {
        cout << "visited CALL node" << endl;


    };

    void visit(ast::Break &node) { };

    void visit(ast::Continue &node) { };

    void visit(ast::Return &node) { };

    void visit(ast::If &node) { };

    void visit(ast::While &node) { };

    void visit(ast::VarDecl &node) { };

    void visit(ast::Assign &node) { };

    void visit(ast::Formal &node) { };

    void visit(ast::Formals &node) { };

    void visit(ast::FuncDecl &node) { };

    void visit(ast::Funcs &node) { };
    
}
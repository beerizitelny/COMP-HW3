#include "nodes.hpp"
#include "output.hpp"
#include "visitor.cpp"
#include "visitor.hpp"

// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();

    SemanticParserVisitor semantic_visitor;
    program->accept(semantic_visitor);
    std::cout << semantic_visitor.symbol_table_stack.scope_printer;
}

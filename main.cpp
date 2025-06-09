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
    try {
        program->accept(semantic_visitor);
        }
    catch(const std::exception& e) {
        std::cerr << "Semantic error" << e.what() << std::endl;}

    std::cout << semantic_visitor.symbol_table_stack.scope_printer;
}

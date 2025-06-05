%{
// #pragma GCC diagnostic ignored "-Wfree-nonheap-object"

#include "nodes.hpp"
#include "output.hpp"

// bison declarations
extern int yylineno;
extern int yylex();

void yyerror(const char*);

// root of the AST, set by the parser and used by other parts of the compiler
std::shared_ptr<ast::Node> program;

using namespace std;

// TODO: Place any additional declarations here
%}

// BISON DECLARATIONS

// TODO: break operators to allow precedence (bottom is prioritized)
%token SC
%token STRING
%token BYTE
%token BOOL
%token VOID
%token TRUE
%token NUM
%token RETURN
%token IF
%token WHILE
%token BREAK
%token CONTINUE
%token FALSE
%token COMMA
%token ID
%token INT
%token NUM_B

// TODO: Define precedence and associativity here
%right ASSIGN

%left OR
%left AND

%left EQ NEQ
%left LT GT LTE GTE

%left PLUS MINUS
%left MULT DIV

%right NOT

%left LPAREN RPAREN
%right ELSE
%left LBRACE RBRACE
%left LBRACK RBRACK

%%

// While reducing the start variable, set the root of the AST
Program:  Funcs { program = $1; }
;

Funcs: {$$ = make_shared<ast::Funcs>();} //epsilon

       | FuncDecl Funcs {
            dynamic_pointer_cast<ast::Funcs>($2)->push_front(dynamic_pointer_cast<ast::FuncDecl>($1));
            $$ = $2;
        }
;

FuncDecl: RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE {
            $$ = make_shared<ast::FuncDecl>(
                dynamic_pointer_cast<ast::ID>($2),
                dynamic_pointer_cast<ast::Type>($1),
                dynamic_pointer_cast<ast::Formals>($4),
                dynamic_pointer_cast<ast::Statements>($7)
            );
        }
;

ExpList: Exp {$$ = make_shared<ast::ExpList>(dynamic_pointer_cast<ast::Exp>($1));}

        | Exp COMMA ExpList {dynamic_pointer_cast<ast::ExpList>($3)->push_front(
            dynamic_pointer_cast<ast::Exp>($1)
        ); $$ = $3;}
;

Formals: {$$ = make_shared<ast::Formals>();} // epsilon

        | FormalsList {$$ = $1;}
;

Statements: Statement {$$ = make_shared<ast::Statements>(dynamic_pointer_cast<ast::Statement>($1));}

        | Statements Statement {dynamic_pointer_cast<ast::Statements>($1)->push_back(
            dynamic_pointer_cast<ast::Statement>($2)
        ); $$ = $1;}
;

FormalsList: FormalsDecl {$$ = make_shared<ast::Formals>(dynamic_pointer_cast<ast::Formal>($1));}

        | FormalsDecl COMMA FormalsList {dynamic_pointer_cast<ast::Formals>($3)->push_front(
            dynamic_pointer_cast<ast::Formal>($1)
        ); $$ = $3;}
;

RetType: Type {$$ = $1;}

        | VOID {$$ = make_shared<ast::PrimitiveType>(ast::BuiltInType::VOID);}
;

Statement: LBRACE Statements RBRACE {$$ = $2;}

        | Call SC {$$ = $1;}

        | Type ID ASSIGN Exp SC {$$ = make_shared<ast::VarDecl>(
            dynamic_pointer_cast<ast::ID>($2),
            dynamic_pointer_cast<ast::Type>($1),
            dynamic_pointer_cast<ast::Exp>($4)
        );}

        | ID LBRACK Exp RBRACK ASSIGN Exp SC {$$ = make_shared<ast::ArrayAssign>(
            dynamic_pointer_cast<ast::ID>($1),
            dynamic_pointer_cast<ast::Exp>($6),
            dynamic_pointer_cast<ast::Exp>($3)
        );}

        | IF LPAREN Exp RPAREN Statement ELSE Statement {$$ = make_shared<ast::If>(
            dynamic_pointer_cast<ast::Exp>($3),
            dynamic_pointer_cast<ast::Statement>($5),
            dynamic_pointer_cast<ast::Statement>($7)
        );}

        | Type ID LBRACK Exp RBRACK SC {$$ = make_shared<ast::VarDecl>(
            dynamic_pointer_cast<ast::ID>($2),
            make_shared<ast::ArrayType>(
                dynamic_pointer_cast<ast::PrimitiveType>($1)->type,
                dynamic_pointer_cast<ast::Exp>($4)
            )
        );}

        | RETURN SC {$$ = make_shared<ast::Return>();}

        | Type ID SC {$$ = make_shared<ast::VarDecl>(
            dynamic_pointer_cast<ast::ID>($2),
            dynamic_pointer_cast<ast::Type>($1)
        );}

        | RETURN Exp SC {$$ = make_shared<ast::Return>(
            dynamic_pointer_cast<ast::Exp>($2)
        );}

        | WHILE LPAREN Exp RPAREN Statement {$$ = make_shared<ast::While>(
            dynamic_pointer_cast<ast::Exp>($3),
            dynamic_pointer_cast<ast::Statement>($5)
        );}

        | IF LPAREN Exp RPAREN Statement {$$ = make_shared<ast::If>(
            dynamic_pointer_cast<ast::Exp>($3),
            dynamic_pointer_cast<ast::Statement>($5)
        );}

        | CONTINUE SC {$$ = make_shared<ast::Continue>();}

        | ID ASSIGN Exp SC {$$ = make_shared<ast::Assign>(
            dynamic_pointer_cast<ast::ID>($1),
            dynamic_pointer_cast<ast::Exp>($3)
        );}

        | BREAK SC {$$ = make_shared<ast::Break>();}
;

FormalsDecl: Type ID {$$ = make_shared<ast::Formal>(
            dynamic_pointer_cast<ast::ID>($2),
            dynamic_pointer_cast<ast::Type>($1)
        );}
;

Call: ID LPAREN ExpList RPAREN {$$ = make_shared<ast::Call>(
            dynamic_pointer_cast<ast::ID>($1),
            dynamic_pointer_cast<ast::ExpList>($3)
        );}

        | ID LPAREN RPAREN {$$ = make_shared<ast::Call>(dynamic_pointer_cast<ast::ID>($1));}
;

Type: INT {$$ = make_shared<ast::PrimitiveType>(ast::INT);}

        | BYTE {$$ = make_shared<ast::PrimitiveType>(ast::BYTE);}

        | BOOL {$$ = make_shared<ast::PrimitiveType>(ast::BOOL);}
;

Exp: LPAREN Exp RPAREN {$$ = $2;}

        | Exp MULT Exp {$$ = make_shared<ast::BinOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::MUL
        );}

        | NUM {$$ = $1;}

        | Exp DIV Exp {$$ = make_shared<ast::BinOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::DIV
        );}

        | TRUE {$$ = make_shared<ast::Bool>(true);}

        | Call {$$ = $1;}

        | Exp PLUS Exp {$$ = make_shared<ast::BinOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::ADD
        );}

        | ID {$$ = $1;}


        | STRING {$$ = $1;}

        | Exp MINUS Exp {$$ = make_shared<ast::BinOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::SUB
        );}

        | NUM_B {$$ = $1;}

        | Exp GTE Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::GE
        );}

        | NOT Exp {$$ = make_shared<ast::Not>(dynamic_pointer_cast<ast::Exp>($2));}

        | Exp EQ Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::EQ
        );}

        | FALSE {$$ = make_shared<ast::Bool>(false);}

        | Exp AND Exp {$$ = make_shared<ast::And>(
            dynamic_pointer_cast<ast::Exp>($1),
             dynamic_pointer_cast<ast::Exp>($3)
         );}

        | Exp LT Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::LT
        );}

        | ID LBRACK Exp RBRACK {$$ = make_shared<ast::ArrayDereference>(
            dynamic_pointer_cast<ast::ID>($1),
            dynamic_pointer_cast<ast::Exp>($3)
        );}

        | Exp NEQ Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::NE
        );}

        | Exp LTE Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::LE
        );}

        | Exp OR Exp {$$ = make_shared<ast::Or>(
            dynamic_pointer_cast<ast::Exp>($1),
             dynamic_pointer_cast<ast::Exp>($3)
         );}

        | LPAREN Type RPAREN Exp {$$ = make_shared<ast::Cast>(
            dynamic_pointer_cast<ast::Exp>($4),
            dynamic_pointer_cast<ast::PrimitiveType>($2)
        );}

        | Exp GT Exp {$$ = make_shared<ast::RelOp>(
            dynamic_pointer_cast<ast::Exp>($1),
            dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::GT
        );}
;


%%

// TODO: Place any additional code here
void yyerror(const char* s) {output::errorSyn(yylineno);}
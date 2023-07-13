#include <vector>
#include <string>
#include <stdexcept>
#include "Token.h"
#include "Expr.h"
#include "Stmt.h"

class Parser final {
private:
    // in book, Parse_Error inherite from runtime_error
    // but here, actually we don't need init and message
    // this somehow work, but may cause other problem
    class Parse_Error {} ;

    std::vector<Token>& tokens;
    int current = 0;

    Expr* expression();
    Stmt* declaration();
    Stmt* statement();
    Stmt* for_statement();
    Stmt* if_statement();
    Stmt* print_statement();
    Stmt* return_statement();
    Stmt* var_declaration();
    Stmt* while_statement();
    Stmt* expression_statement();
    Stmt::Function* function(std::string kind);
    std::vector<Stmt*> block();
    Expr* assignment();
    Expr* lox_or();
    Expr* lox_and();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* finish_call(Expr* callee);
    Expr* call();
    Expr* primary();
    // consume token if in types, otherwise return false
    bool match(std::vector<TokenType> types);
    Token consume(TokenType type, std::string message);
    bool check(TokenType type);
    Token advance();
    // checks if we’ve run out of tokens to parse
    bool is_at_end();
    // returns the current token we have yet to consume
    Token peek();
    // access the just-matched token
    Token previous();
    Parse_Error error(Token token, std::string message);
    void synchronize();
public:
    Parser(std::vector<Token>& tokens);
    // Expr* parse();
    std::vector<Stmt*> parse();
};
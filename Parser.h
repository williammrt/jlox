#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
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

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> for_statement();
    std::unique_ptr<Stmt> if_statement();
    std::unique_ptr<Stmt> print_statement();
    std::unique_ptr<Stmt> return_statement();
    std::unique_ptr<Stmt> var_declaration();
    std::unique_ptr<Stmt> while_statement();
    std::unique_ptr<Stmt> expression_statement();
    std::unique_ptr<Stmt::Function> function(std::string kind);
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> lox_or();
    std::unique_ptr<Expr> lox_and();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> finish_call(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();
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
    // std::unique_ptr<Expr> parse();
    std::vector<std::unique_ptr<Stmt>> parse();
};
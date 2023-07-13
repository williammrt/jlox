#include <iostream>

#include "Parser.h"
#include "Expr.h"
#include "Lox.h"

Expr* Parser::expression() {
    // return equality();
    return assignment();
}

Stmt* Parser::declaration() {
    try {
        if (match({FUN})) {
            return function("function");
        }
        if (match({VAR})) { 
            return var_declaration();
        }
        return statement();
    } catch (Parse_Error& error) {
        synchronize();
        return nullptr;
    }   
}

Stmt* Parser::statement() {
    if (match({FOR})) {
        return for_statement();
    }
    if (match({IF})) {
        return if_statement();
    }
    if (match({PRINT})) {
        return print_statement();
    }
    if (match({RETURN})) {
        return return_statement();
    }
    if (match({WHILE})) {
        return while_statement();
    }
    if (match({LEFT_BRACE})) {
        return new Stmt::Block(block());
    }
    return expression_statement();
}

Stmt* Parser::for_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    Stmt* initializer;
    if (match({SEMICOLON})) {
      initializer = nullptr;
    } else if (match({VAR})) {
      initializer = var_declaration();
    } else {
      initializer = expression_statement();
    }

    Expr* condition = nullptr;
    if (!check(SEMICOLON)) {
      condition = expression();
    }
    consume(SEMICOLON, "Expect ';' after loop condition.");

    Expr* increment = nullptr;
    if (!check(RIGHT_PAREN)) {
      increment = expression();
    }
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    Stmt* body = statement();

    if (increment) {
        body = new Stmt::Block({body, new Stmt::Expression(increment)});
    }

    if (!condition) {
        condition = new Expr::Literal(true);
    }
    body = new Stmt::While(condition, body);

    if (initializer) {
        body = new Stmt::Block({initializer, body});
    }

    return body;

}

Stmt* Parser::if_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition."); 

    Stmt* then_branch = statement();
    Stmt* else_branch = nullptr;
    if (match({ELSE})) {
        else_branch = statement();
    }

    return new Stmt::If(condition, then_branch, else_branch);
}

Stmt* Parser::print_statement() {
    Expr* value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return new Stmt::Print(value);
}

Stmt* Parser::return_statement() {
    Token keyword = previous();
    Expr* value = nullptr;
    if (!check(SEMICOLON)) {
        value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return new Stmt::Return(keyword, value);
}
Stmt* Parser::var_declaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");

    Expr* initializer = nullptr;
    if (match({EQUAL})) {
        initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return new Stmt::Var(name, initializer);
}

Stmt* Parser::while_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");
    Stmt* body = statement();

    return new Stmt::While(condition, body);
}

Stmt* Parser::expression_statement() {
    Expr* expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return new Stmt::Expression(expr);
}

Stmt::Function* Parser::function(std::string kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }

            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<Stmt*> body = block();
    return new Stmt::Function(name, parameters, body);
}

std::vector<Stmt*> Parser::block() {
    std::vector<Stmt*> statements;

    while (!check(RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

Expr* Parser::assignment() {
    // Expr* expr = equality();
    Expr* expr = lox_or();

    if (match({EQUAL})) {
        Token equals = previous();
        Expr* value = assignment();

        if (dynamic_cast<Expr::Variable*>(expr)) {
            Token name = dynamic_cast<Expr::Variable*>(expr)->name;
            return new Expr::Assign(name, value);
        }

        error(equals, "Invalid assignment target."); 
    }

    return expr;
}

Expr* Parser::lox_or() {
    Expr* expr = lox_and();

    while(match({OR})) {
        Token op = previous();
        Expr* right = lox_and();
        expr = new Expr::Logical(expr, op, right);
    }

    return expr;
}

Expr* Parser::lox_and() {
    Expr* expr = equality();

    while(match({AND})) {
        Token op = previous();
        Expr* right = equality();
        expr = new Expr::Logical(expr, op, right);
    }

    return expr;
}

Expr* Parser::equality() {
    Expr* expr = comparison();

    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        Expr* right = comparison();
        // notice how we construct tree, so it is left assiciative
        expr = new Expr::Binary(expr, op, right);
    }

    return expr;
}

Expr* Parser::comparison() {
    Expr* expr = term();

    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        Token op = previous();
        Expr* right = term();
        expr = new Expr::Binary(expr, op, right);      
    }

    return expr;
}

Expr* Parser::term() {
    Expr* expr = factor();

    while (match({MINUS, PLUS})) {
        Token op = previous();
        Expr* right = factor();
        expr = new Expr::Binary(expr, op, right);      
    }

    return expr;
}

Expr* Parser::factor() {
    Expr* expr = unary();

    while (match({SLASH, STAR})) {
        Token op = previous();
        Expr* right = unary();
        expr = new Expr::Binary(expr, op, right);      
    }

    return expr;
}

Expr* Parser::unary() {
    if (match({BANG,MINUS})) {
        Token op = previous();
        Expr* right = unary();
        return new Expr::Unary(op, right);
    }

    // return primary();
    return call();
}

Expr* Parser::finish_call(Expr* callee) {
    std::vector<Expr*> arguments;
    if (!check(RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({COMMA}));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return new Expr::Call(callee, paren, arguments);
}

Expr* Parser::call() {
    Expr* expr = primary();

    while(true) {
        if (match({LEFT_PAREN})) {
            expr = finish_call(expr);
        } else {
            break;
        }
    }
    return expr;
}

Expr* Parser::primary() {
    if (match({FALSE})) { return new Expr::Literal(false); }
    if (match({TRUE})) { return new Expr::Literal(true); }
    if (match({NIL})) { return new Expr::Literal(Nil{}); }

    if (match({NUMBER, STRING})) {
        return new Expr::Literal(previous().literal);
    }

    if (match({IDENTIFIER})) {
      return new Expr::Variable(previous());
    }

    if (match({LEFT_PAREN})) {
        Expr* expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return new Expr::Grouping(expr);
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type: types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) {
        return advance();
    }

    throw error(peek(), message);
}

bool Parser::check(TokenType type) {
    if (is_at_end()) {
        return false;
    }
    return peek().type == type;
}

Token Parser::advance() {
    if (!is_at_end()) {
        current += 1;
    }
    return previous();
}

bool Parser::is_at_end() {
    // Well, we should make a getter
    return peek().type == EOF_T; 
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current-1];
}

Parser::Parse_Error Parser::error(Token token, std::string message) {
    ::error(token, message);
    return Parse_Error();
}

void Parser::synchronize() {
    advance();

    while (!is_at_end()) {
        if (previous().type == SEMICOLON) {
            return;
        }

        switch (peek().type) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}

Parser::Parser(std::vector<Token>& tokens): tokens{tokens} {}

/*
Expr* Parser::parse() {
    try {
        return expression();
    } catch (Parse_Error& error) { 
        return nullptr;
    }
}
*/

std::vector<Stmt*> Parser::parse() {
    std::vector<Stmt*> statements;
    while (!is_at_end()) {
        // statements.push_back(statement());
        statements.push_back(declaration());
    }

    return statements;
}
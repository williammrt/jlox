#include <iostream>

#include "Parser.h"
#include "Expr.h"
#include "Lox.h"

std::unique_ptr<Expr> Parser::expression() {
    // return equality();
    return assignment();
}

std::unique_ptr<Stmt> Parser::declaration() {
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

std::unique_ptr<Stmt> Parser::statement() {
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
        return std::make_unique<Stmt::Block>(block());
    }
    return expression_statement();
}

std::unique_ptr<Stmt> Parser::for_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match({SEMICOLON})) {
      initializer = nullptr;
    } else if (match({VAR})) {
      initializer = var_declaration();
    } else {
      initializer = expression_statement();
    }

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(SEMICOLON)) {
        condition = expression();
    }
    consume(SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(RIGHT_PAREN)) {
        increment = expression();
    }
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    std::unique_ptr<Stmt> body = statement();

    if (increment) {
        std::vector<std::unique_ptr<Stmt>> args;
        args.emplace_back(move(body));
        std::unique_ptr<Stmt> tmp;
        tmp.reset(new Stmt::Expression(move(increment)));
        args.emplace_back(move(tmp));
        body.reset(new Stmt::Block(std::move(args)));
    }

    if (!condition) {
        condition.reset(new Expr::Literal(true));

    }


    body.reset(new Stmt::While(std::move(condition), std::move(body)));

    if (initializer) {
        std::vector<std::unique_ptr<Stmt>> args;
        args.emplace_back(std::move(initializer));
        args.emplace_back(std::move(body));
        body.reset(new Stmt::Block(move(args)));
    }

    return body;

}

std::unique_ptr<Stmt> Parser::if_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition."); 

    std::unique_ptr<Stmt> then_branch = statement();
    std::unique_ptr<Stmt> else_branch = nullptr;
    if (match({ELSE})) {
        else_branch = statement();
    }

    return std::make_unique<Stmt::If>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::print_statement() {
    std::unique_ptr<Expr> value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Stmt::Print>(std::move(value));
}

std::unique_ptr<Stmt> Parser::return_statement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(SEMICOLON)) {
        value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<Stmt::Return>(keyword, std::move(value));
}
std::unique_ptr<Stmt> Parser::var_declaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({EQUAL})) {
        initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Stmt::Var>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::while_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<Stmt::While>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::expression_statement() {
    std::unique_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Stmt::Expression>(std::move(expr));
}

std::unique_ptr<Stmt::Function> Parser::function(std::string kind) {
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
    std::vector<std::unique_ptr<Stmt>> body = block();
    return std::make_unique<Stmt::Function>(name, parameters, std::move(body));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Expr> Parser::assignment() {
    // std::unique_ptr<Expr> expr = equality();
    std::unique_ptr<Expr> expr = lox_or();

    if (match({EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (dynamic_cast<Expr::Variable*>(expr.get())) {
            Token name = dynamic_cast<Expr::Variable*>(expr.get())->name;
            return std::make_unique<Expr::Assign>(name, std::move(value));
        }

        error(equals, "Invalid assignment target."); 
    }

    return expr;
}

std::unique_ptr<Expr> Parser::lox_or() {
    std::unique_ptr<Expr> expr = lox_and();

    while(match({OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = lox_and();
        expr.reset(new Expr::Logical(std::move(expr), op, std::move(right)));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::lox_and() {
    std::unique_ptr<Expr> expr = equality();

    while(match({AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr.reset(new Expr::Logical(std::move(expr), op, std::move(right)));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        // notice how we construct tree, so it is left assiciative
        expr.reset(new Expr::Binary(std::move(expr), op, std::move(right)));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr.reset(new Expr::Binary(std::move(expr), op, std::move(right)));      
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({MINUS, PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr.reset(new Expr::Binary(std::move(expr), op, std::move(right)));      
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({SLASH, STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr.reset(new Expr::Binary(std::move(expr), op, std::move(right)));      
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({BANG,MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Expr::Unary>(op, std::move(right));
    }

    // return primary();
    return call();
}

std::unique_ptr<Expr> Parser::finish_call(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({COMMA}));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<Expr::Call>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while(true) {
        if (match({LEFT_PAREN})) {
            expr = finish_call(move(expr));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({FALSE})) { return std::make_unique<Expr::Literal>(false); }
    if (match({TRUE})) { return std::make_unique<Expr::Literal>(true); }
    if (match({NIL})) { return std::make_unique<Expr::Literal>(Nil{}); }

    if (match({NUMBER, STRING})) {
        return std::make_unique<Expr::Literal>(previous().literal);
    }

    if (match({IDENTIFIER})) {
      return std::make_unique<Expr::Variable>(previous());
    }

    if (match({LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Expr::Grouping>(std::move(expr));
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
std::unique_ptr<Expr> Parser::parse() {
    try {
        return expression();
    } catch (Parse_Error& error) { 
        return nullptr;
    }
}
*/

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end()) {
        // statements.push_back(statement());
        statements.push_back(declaration());
    }

    return statements;
}
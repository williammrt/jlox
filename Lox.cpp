#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

#include "Lox.h"
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "Expr.h"
#include "Interpreter.h"
#include "Resolver.h"

using std::string, std::cout, std::cerr;

Interpreter interpreter;

bool hadError = false;
bool hadRuntimeError = false;

int main(int argc, char** argv) {
    /* 
    Expr* expression = new Expr::Binary(
        new Expr::Unary(
            Token(MINUS, "-", {}, 1),
            new Expr::Literal(double(123))),
        Token(STAR, "*", {}, 1),
        new Expr::Grouping(new Expr::Literal(45.67))
    );
    std::cout << (new Ast_Printer)->print(expression);
    */

    if (argc == 1) {
        runPrompt();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        cout << "Usage: jlox [script]\n";
        std::exit(64);
    }
    
}

void runFile(const char *fileName) {
    std::ifstream inFile(fileName);
    string source = string(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>());
    inFile.close();

    // cout << source << '\n';
    run(source);
    // InterpretResult result = interpret(std::move(buffer.str()));
    if (hadError) {
        std::exit(65);
    }
}

void runPrompt() {
    string line;
    while (true) {
        cout << "> ";
        if (!std::getline(std::cin, line)) {
            cout << "\n";
            break;
        }
        hadError = false;
    }
}

void run(string source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    /*
    cout << "\nScanner\n";
    for (Token token: tokens) {
        cout << token.to_string() << "\n";
    }
    */
    
    Parser parser(tokens);
    // Expr* expression = parser.parse();
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    // Stop if there was a syntax error.
    if (hadError) { return; }
    
    Resolver resolver(interpreter);
    resolver.resolve(statements);
    if (hadError) { return; }
    // cout << "Ast_Printer: " <<Ast_Printer().print(expression) << '\n';

    // interpreter.interpret(expression);
    interpreter.interpret(std::move(statements));
    if (hadRuntimeError) { std::exit(70); }
}

void error(int line, string message) {
    report(line, "", message);
}

void report(int line, string where, string message) {
    cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
    
    hadError = true;
}

void error(Token token, std::string message) {
    if (token.type == EOF_T) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" +  token.lexeme + "'", message);
    }
}

void report_runtime_error(Lox_runtime_error& error) {
    std::cerr << error.what() << "\n[line "
        << error.token.line << "]\n";
    std::cerr << error.token.to_string() << '\n';
    hadRuntimeError = true;
}

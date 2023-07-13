#ifndef Lox_H
#define Lox_H

#include "Token.h"
#include "Runtime_Error.h"

void runFile(const char *fileName);
void runPrompt();
void run(std::string source);
void error(int line, std::string message);
void report(int line, std::string where, std::string message);
void error(Token token, std::string message);
void report_runtime_error(Lox_runtime_error& error);

#endif
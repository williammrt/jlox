#ifndef Runtime_Error_H
#define Runtime_Error_H

#include <stdexcept>
#include <string>
#include "Token.h"

class Lox_runtime_error final: public std::runtime_error {
public:
    Token token;
    Lox_runtime_error(Token token, std::string message);
};

#endif

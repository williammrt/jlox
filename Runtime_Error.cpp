#include "Runtime_Error.h"

Lox_runtime_error::Lox_runtime_error(Token token, std::string message)
    :runtime_error{message}, token {token} {} 
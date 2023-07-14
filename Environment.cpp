#include "Environment.h"

Environment::Environment() {
    enclosing = nullptr;
}

Environment::Environment(Environment* enclosing)
    :enclosing {enclosing} {}


Object Environment::get(Token name) {
    if (values.count(name.lexeme)) {
        return values[name.lexeme];
    }

    if (enclosing) {
        return enclosing->get(name);
    }

    throw Lox_runtime_error(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(Token name, Object value) {
    if (values.count(name.lexeme)) {
        values[name.lexeme] = value;
        return;
    }

    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }

    throw Lox_runtime_error(name,
        "Undefined variable '" + name.lexeme + "'.");
}

void Environment::define(std::string name, Object value) {
    values[name] = value;
}
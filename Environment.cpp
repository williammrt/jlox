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

Environment* Environment::ancestor(int distance) {
    Environment* environment = this;
    for (int i = 0; i < distance; i += 1) {
        environment = environment->enclosing;
    }

    return environment;
}

Object Environment::get_at(int distance, std::string name) {
    return ancestor(distance)->values[name];
}

void Environment::assign_at(int distance, Token name, Object value) {
    ancestor(distance)->values[name.lexeme] = value;
}
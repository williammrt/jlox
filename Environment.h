#ifndef Environment_H
#define Environment_H

#include <unordered_map>
#include <string>
#include "Object.h"
#include "Token.h"
#include "Runtime_Error.h"

class Environment final {
private:
    Environment* enclosing;
    std::unordered_map<std::string, Object> values;
public:
    Environment();
    Environment(Environment* enclosing);
    Object get(Token name);
    void assign(Token name, Object value);
    void define(std::string name, Object value);
    Environment* ancestor(int distance);
    Object get_at(int distance, std::string name);
    void assign_at(int distance, Token name, Object value);
};

#endif
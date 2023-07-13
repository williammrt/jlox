#ifndef Lox_Callable_h
#define Lox_Callable_h

#include <vector>
#include <string>
#include "Interpreter.h"
#include "Object.h"

class Lox_callable {
public:
    virtual int arity() = 0;
    virtual Object call(Interpreter& interpreter, std::vector<Object> arguments) = 0;
    virtual std::string to_string() = 0;
};

class native_clock final: public Lox_callable {
public:
    int arity() override;
    Object call(Interpreter& interpreter, std::vector<Object> arguments) override;
    std::string to_string() override;
};

class Lox_function final: public Lox_callable {
private:
    Stmt::Function* declaration;
    Environment* closure;
public:
    Lox_function(Stmt::Function* declaration, Environment* closure);
    int arity() override;
    Object call(Interpreter& interpreter, std::vector<Object> arguments) override;
    std::string to_string() override;
};

#endif
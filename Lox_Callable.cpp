#include <chrono>
#include "Lox_Callable.h"
#include "Return.h"

int native_clock::arity() {
    return 0;
}

Object native_clock::call(Interpreter& interpreter, std::vector<Object> arguments) {
    auto now = std::chrono::system_clock::now();
    double now_double = std::chrono::duration<double>(now.time_since_epoch()).count();
    return now_double;
}

std::string native_clock::to_string() {
    return "<native fn>";
}

Lox_function::Lox_function(Stmt::Function* declaration, Environment* closure)
//    :declaration {declaration}, closure {new Environment(*closure)} {}
    :declaration {declaration}, closure {closure} {}

int Lox_function::arity() {
    return static_cast<int>((declaration->params).size());
}

Object Lox_function::call(Interpreter& interpreter, std::vector<Object> arguments) {
    // line below is bug, it calls a copy constructor, actually we want to use constructor that closure is pointer input
    // Environment* environment = new Environment(*closure); 
    Environment* environment = new Environment(closure);
    for (int i = 0; i < static_cast<int>((declaration->params).size()); i +=1)  {
        environment->define((declaration->params)[i].lexeme, arguments[i]);
    }

    try {
        interpreter.execute_block(declaration->body, environment);
    } catch (Return& return_value) {
        return return_value.value;
    }
    
    return Nil{};
}

std::string Lox_function::to_string() {
    return "<fn " + (declaration->name).lexeme + ">";
}
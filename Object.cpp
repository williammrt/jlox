#include "Object.h"
#include "Lox_Callable.h"

std::string stringify_Object(Object value) {
    std::string value_str;
    switch (value.index()) {
        case 0:
            value_str = "void";
            break;
        case 1:
            value_str = "nil";
            break;
        case 2:
            value_str = std::to_string(std::get<double>(value));
            // https://stackoverflow.com/questions/13686482/c11-stdto-stringdouble-no-trailing-zeros
            value_str.erase ( value_str.find_last_not_of('0') + 1, std::string::npos );
            value_str.erase ( value_str.find_last_not_of('.') + 1, std::string::npos );
            break;
        case 3:
            value_str = std::get<std::string>(value);
            break;
        case 4: 
            if (std::get<bool>(value)) {
                value_str = "true";
            } else {
                value_str = "false";
            }
            break;
        case 5: 
            value_str = std::get<Lox_callable*>(value)->to_string();
    }
    return value_str;
}
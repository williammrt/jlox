#ifndef Object_H
#define Object_H

#include <variant>
#include <string>
// #include "Lox_Callable.h"


// TWL: dirty hack
class Lox_callable;

class Nil {};

using Object = std::variant<std::monostate, Nil, double, std::string, bool, Lox_callable*>;

std::string stringify_Object(Object value);

#endif
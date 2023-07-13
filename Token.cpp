#include <string>
#include "Token.h"
#include "Object.h"

using std::string;

Token::Token(TokenType type, std::string_view lexeme, Object literal, int line)
    :type{type}, lexeme{lexeme}, literal{literal}, line{line} {}

string Token::to_string() const {
    string literal_str;
    switch (literal.index()) {
        case 0:
            literal_str = "void";
            break;
        case 1:
            literal_str = "nil";
            break;
        case 2:
            literal_str = std::to_string(std::get<double>(literal));
            break;
        case 3:
            literal_str = std::get<std::string>(literal);
            break;
        case 4: 
            if (std::get<bool>(literal)) {
                literal_str = "true";
            } else {
                literal_str = "false";
            }
            break;
    }

    return std::to_string(static_cast<int>(type)) + " " + lexeme + " " + literal_str;
}
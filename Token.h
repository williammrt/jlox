#ifndef Token_H
#define Token_H

#include <string>
#include <variant>
#include "Object.h"

enum TokenType {
	// Single-character tokens.
	// 0 .. 10
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	// One or two character tokens.
	// 11 .. 18
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	// Literals.
	// 19 .. 21
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	// 22 .. 37
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	// 38
	EOF_T
};

class Token final {
public:
	TokenType type;
	std::string lexeme;
	Object literal;
	int line;

	Token(TokenType type, std::string_view lexeme, Object literal, int line);
	std::string to_string() const;
};


#endif
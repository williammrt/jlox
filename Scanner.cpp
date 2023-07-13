#include <cctype>
#include <string>
#include "Scanner.h"
#include "Token.h"
#include "Lox.h"

Scanner::Scanner(std::string& source): source{source} {
    keywords["and"] =     AND;
    keywords["class"] =   CLASS;
    keywords["else"] =    ELSE;
    keywords["false"] =   FALSE;
    keywords["for"] =     FOR;
    keywords["fun"] =     FUN;
    keywords["if"] =      IF;
    keywords["nil"] =     NIL;
    keywords["or"] =      OR;
    keywords["print"] =   PRINT;
    keywords["return"] =  RETURN;
    keywords["super"] =   SUPER;
    keywords["this"] =    THIS;
    keywords["true"] =    TRUE;
    keywords["var"] =     VAR;
    keywords["while"] =   WHILE;
}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(EOF_T , {}, {}, line));
    return tokens;
}

void Scanner::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break; 
        case '!':
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            if (match('/')) {
            // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) { advance(); }
            } else {
                addToken(SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            break;

        
        case '"': string(); break;

        default:
            if (std::isdigit(c)) {
                number();
            } else if (std::isalpha(c)||c=='_') {
                identifier();
            } else {
                error(line, "Unexpected character.");
            }
            break;
    }
}

void Scanner::identifier() {
    while (std::isalnum(peek()) || peek()=='_') { advance(); }

    std::string text = source.substr(start, current-start);
    TokenType type;
    if (keywords.count(text)) {
        type = keywords[text];
    } else {
        type = IDENTIFIER;
    }
    addToken(type);
}

void Scanner::number() {
    while (std::isdigit(peek())) { advance(); }

    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (std::isdigit(peek())) { advance(); }
    }
    // forgot stod.... cause me great trouble
    // how to check logical error cased type error?
    addToken(NUMBER, std::stod(source.substr(start, current-start)));
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { line+= 1; }
        advance();
    }

    if (isAtEnd()) {
        error(line, "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    std::string value = source.substr(start+1, current-start-2);
    addToken(STRING, value);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) { return false; }
    if (source[current] != expected) { return false; }

    current += 1;
    return true;
}

char Scanner::peek() {
    if (isAtEnd()) { return '\0'; }
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= static_cast<int>(source.size())) { return '\0'; }
    return source[current+1];
}

bool Scanner::isAtEnd() {
    return current >= static_cast<int>(source.size());
}

char Scanner::advance() {
    current += 1;
    return source[current-1];
}

void Scanner::addToken(TokenType type) {
    addToken(type, {});
}
void Scanner::addToken(TokenType type, Object literal) {
    std::string text = source.substr(start, current-start);
    tokens.push_back(Token(type, text, literal, line));
}

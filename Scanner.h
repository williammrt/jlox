#include <string>
#include <vector>
#include <unordered_map>
#include "Token.h"
#include "Object.h"

class Scanner final {
private:
    std::string& source;
    std::vector<Token> tokens;
    std::unordered_map<std::string, TokenType> keywords;
    int start = 0;  // start of token's lexeme
    int current = 0; // like PC
    int line = 1;
public:
    Scanner(std::string& source);
    std::vector<Token> scanTokens();
    void scanToken();
    void identifier();
    void number();
    void string();
    bool match(char expected); // like conditional advance()
    char peek(); // lookahead
    char peekNext();
    bool isAtEnd();
    char advance(); 
    void addToken(TokenType type);
    void addToken(TokenType type, Object literal);
};
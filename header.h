#include <bits/stdc++.h>

using namespace std;

enum class Bundle_type { STRING, NUM, BOOL, NIL, VOID };

class Bundle {
public:
  Bundle_type type;
  string str;
  double num;
  bool boo;

  Bundle(Bundle_type type = Bundle_type::VOID, string str = "", double num = 0,
         bool boo = false);
  string toString() const;
  bool isEqual(Bundle other) const;
};

// use enum instead of enum class because we can easily print out
enum TokenType {

  // Single-character tokens. 0
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens. 11
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals. 19
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords. 22
  AND,
  CLASS,
  ELSE,
  FALSE_T,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE_T,
  VAR,
  WHILE,

  EOF_T // _T for name clash, 38
};

class Token {
public:
  TokenType type;
  string lexeme;
  Bundle literal; // TODO
  int line;
  Token(TokenType type, string lexeme, Bundle literial, int line);
  friend ostream &operator<<(ostream &os, const Token &TokenType);
};

class RuntimeError : public exception {
public:
  string message;
  Token token;
  RuntimeError(Token token, string message);
};

class Environment {
//private:
public:
    unordered_map<string, Bundle> values;
    Environment* enclosing;
    int eid;
    
    void define(string name, Bundle value);
    Bundle get(Token name);
    void assign(Token name, Bundle value);
    explicit Environment(Environment* enclosing = nullptr);
};

class Scannar {
private:
  string source;
  unordered_map<string, TokenType> keywords;
  vector<Token> tokens;
  int start = 0;   // points to the first character in the lexeme being scanned
  int current = 0; // points at the character currently being considered
  int line = 1;    //

  bool isAtEnd();

public:
  Scannar(string source);

  void init_keywords();

  vector<Token> ScanTokens();

  void scanToken();

  // get the current char and move forward
  char advance();

  // check whether the current char (called by prev char) equal to expected char
  // only consume the current character if it’s what we’re looking for
  bool match(char expected);

  // only get the current char
  char peek();

  void convert_to_string();

  bool isDigit(char c);

  void handle_number();

  char peekNext();

  void identifier();

  bool isAlpha(char c);

  bool isAlphaNumeric(char c);

  // add token only with type(1 char token)
  void addToken(TokenType type);

  void addToken(TokenType type, Bundle literial);
};

class Visitor;

class Expr {
public:
  virtual Bundle accept(Visitor *visitor) = 0;
};

class Binary : public Expr {
public:
  Binary(Expr *left, Token optor, Expr *right);
  Bundle accept(Visitor *visitor) override;
  Expr *left;
  Token optor;
  Expr *right;
};

class Grouping : public Expr {
public:
  Grouping(Expr *expression);
  Bundle accept(Visitor *visitor) override;
  Expr *expression;
};

class Literal : public Expr {
public:
  Literal(Bundle value);

  Bundle accept(Visitor *visitor) override;

  Bundle value;
};

class Logical: public Expr {
public:
    Expr* left;
    Token optor;
    Expr* right;
    Logical(Expr* left, Token optor, Expr* right);
    Bundle accept(Visitor *visitor) override;
};

class Unary : public Expr {
public:
  Unary(Token optor, Expr *right);

  Bundle accept(Visitor *visitor) override;

  Token optor;
  Expr *right;
};

class Variable : public Expr {
public:
  Variable(Token name);

  Bundle accept(Visitor *visitor) override;

  Token name;
};

class Assign: public Expr {
public:
    Token name;
    Expr* value;
    Assign(Token name, Expr* value);
    Bundle accept(Visitor* visitor) override;
};

class Call: public Expr {
public:
    Expr* callee;
    Token paren;
    vector<Expr*> argument;
    
    Call(Expr* callee, Token paren, vector<Expr*> argument);
    Bundle accept(Visitor* visitor) override;
};

class stmt_Visitor; 

class Stmt {
public:
    virtual Bundle accept(stmt_Visitor* visitor) = 0;  
};

class Block: public Stmt {
public:
    vector<Stmt*> statements;
    Block(vector<Stmt*> statements);
    Bundle accept(stmt_Visitor* visitor) override;
};

class Expression: public Stmt  {
public:
    Expression(Expr* expression);  
    Expr* expression;
    Bundle accept(stmt_Visitor* visitor) override;
};

class Print: public Stmt {
public:
    Print(Expr* expression);  
    Expr* expression;
    Bundle accept(stmt_Visitor* visitor) override;
};

class Var: public Stmt {
public:
    Token name;
    Expr* initializer;
    Var(Token name, Expr* initializer);  
    Bundle accept(stmt_Visitor* visitor) override;
};

class If: public Stmt {
public:
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
    If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
    Bundle accept(stmt_Visitor* visitor) override;
};

class While: public Stmt {
public:
    Expr* condition;
    Stmt* body;
    While(Expr* condition, Stmt* body);
    Bundle accept(stmt_Visitor* visitor) override;
};

class stmt_Visitor {
public:
    virtual Bundle visitExpressionStmt(Expression* stmt) = 0;
    virtual Bundle visitPrintStmt(Print* stmt) = 0;
    virtual Bundle visitVarStmt(Var* stmt) = 0;
    virtual Bundle visitBlockStmt(Block* stmt) = 0;
    virtual Bundle visitIfStmt(If* stmt) = 0;
    virtual Bundle visitWhileStmt(While* stmt) = 0;
};

class Visitor {
public:
  virtual Bundle visitBinaryExpr(Binary *expr) = 0;
  virtual Bundle visitGroupingExpr(Grouping *expr) = 0;
  virtual Bundle visitLiteralExpr(Literal *expr) = 0;
  virtual Bundle visitUnaryExpr(Unary *expr) = 0;
  virtual Bundle visitVariableExpr(Variable* expr) = 0;
  virtual Bundle visitAssignExpr(Assign* expr) = 0;
  virtual Bundle visitLogicalExpr(Logical* expr) = 0;
  virtual Bundle visitCallExpr(Call* expr) = 0;
};

class AstPrinter : public Visitor {
public:
  string print(Expr *expr);
  string parenthesize(string name, vector<Expr *> exprs);

  Bundle visitBinaryExpr(Binary *expr) override;
  Bundle visitGroupingExpr(Grouping *expr) override;
  Bundle visitLiteralExpr(Literal *expr) override;
  Bundle visitUnaryExpr(Unary *expr) override;
};

class Interpreter : public Visitor, public stmt_Visitor {
public:
    Environment* environment;
  explicit Interpreter();
  Bundle evaluate(Expr *expr);
  bool isTruthy(Bundle obj) const;
  bool isEqual(Bundle a, Bundle b) const;
  void checkNumberOperand(Token optor, Bundle operand);
  void checkNumberOperands(Token optor, Bundle left, Bundle right);
  //void interpret(Expr *expr);
  void interpret(vector<Stmt*> statements);
  void execute(Stmt* stmt);
  void executeBlock(vector<Stmt*> statements, Environment* environment);
  Bundle visitBinaryExpr(Binary *expr) override;
  Bundle visitGroupingExpr(Grouping *expr) override;
  Bundle visitLiteralExpr(Literal *expr) override;
  Bundle visitUnaryExpr(Unary *expr) override;
  Bundle visitVariableExpr(Variable* expr) override;
  Bundle visitAssignExpr(Assign* expr) override;
  Bundle visitLogicalExpr(Logical* expr) override;
  Bundle visitCallExpr(Call* expr) override;
  
  Bundle visitExpressionStmt(Expression* stmt) override;
  Bundle visitPrintStmt(Print* stmt) override;
  Bundle visitVarStmt(Var* stmt) override;
  Bundle visitBlockStmt(Block* stmt) override;
  Bundle visitIfStmt(If* stmt) override;
  Bundle visitWhileStmt(While* stmt) override;
};

class ParseError : public exception {};
class Parser {
private:
  vector<Token> tokens;
  int current;
  Expr *expression();
  Expr *equality();
  bool match(vector<TokenType> types);
  bool check(TokenType type);
  Token advance();
  bool isAtEnd();
  Token peek();
  Token previous();
  Expr *comparison();
  Expr *factor();
  Expr *term();

  Expr *unary();
  Expr *primary();
  Token consume(TokenType type, string message);

  ParseError error(Token token, string message);
  void synchronize();
  
  Stmt* statement();
  Stmt* forStatement();
  Stmt* printStatement();
  Stmt* expressionStatement();
  vector<Stmt*> block();
  Stmt* declaration();
  Stmt* varDeclaration();
  Stmt* ifStatement();
  Expr* assignment();
  Expr* Or();
  Expr* And();
  Stmt* whileStatement();
  
  Expr* call();
  Expr* finishCall(Expr* callee);
  // https://www.tutorialspoint.com/cplusplus/cpp_exceptions_handling.htm
public:
  Parser(vector<Token> tokens);
  //Expr *parse();
  vector<Stmt*> parse();
};

void runFile(const char *fileName);
void runPrompt();
void run(string source);
void error(int line, string message);
void runtimeError(RuntimeError err);
void report(int line, string where, string message);
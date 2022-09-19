#include <bits/stdc++.h>
#include "header.h"

const bool INTERPRETER_TRACE = false;
const bool LEXER_TRACE = false;


using namespace std;
void check_values(Environment* e) {
    cout << "check_values eid = " << e->eid << '\n';
    for (auto p:e->values) {
        cout << p.first << p.second.toString() << '\n';
    }
}

bool hasError = false;
bool hadRuntimeError = false;
Interpreter * interpreter = nullptr;

int main(int argc, char * argv[]) {
    interpreter = new Interpreter;
    // test();
    if (argc > 2) {
        cout << "Usage: jlox [script]\n";
        exit(64); // "The command was used incorrectly"
    } else if (argc == 2) { // in C++, argv[0] is executable's name
        runFile(argv[1]);
    } else {
        runPrompt();
    }
    return 0;
}

// treat the source file as a string, as in CI
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
void runFile(const char * fileName) {
    ifstream inFile(fileName);
    stringstream buffer;
    buffer << inFile.rdbuf();
    run(buffer.str());
    inFile.close();
    if (hasError) {
        exit(65);
    }
    if (hadRuntimeError) {
        exit(70);
    }
}

// run line by line
void runPrompt() {
    string line;

    while (getline(cin, line)) {
        if (line.empty()) {
            cout << "finished!\n";
            break;
        }
        cout << "> ";
        run(line);
        hasError = false;
    }
}

void run(string source) {
    Scannar scannar(source);
    vector < Token > tokens = scannar.ScanTokens();
    
    if (LEXER_TRACE) {
        for (Token token: tokens) {
            cout << token << '\n';
        }
    }
    Parser parser(tokens);
    //cout << "After initialized parser \n";
    vector<Stmt*> statements = parser.parse();
    //cout << "After calling parser.parse()\n";
    if (hasError) {
        cout << " has error after calling parser.parse()\n";
        return;
    }
    interpreter -> interpret(statements);
    
    cout << "\nafter interpreter, finished successfully!\n";
    
}

void error(int line, string message) {
    report(line, "", message);
}
void runtimeError(RuntimeError err) {
    cout << err.message << "\n[line " << err.token.line << "]";
    hadRuntimeError = true;
}

void report(int line, string where, string message) {
    cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
    hasError = true;
}

Token::Token(TokenType type, string lexeme, Bundle literal, int line): type {type}, lexeme {lexeme}, literal {literal}, line {line} {}

ostream & operator << (ostream & os, const Token & token) {
    os << token.type << " : " << token.lexeme << " : " << token.literal.toString() << " : " << token.line;
    return os;
}

Scannar::Scannar(string source): source {source} {
    init_keywords();
}

void Scannar::init_keywords() {
    keywords["and"] = TokenType::AND;
    keywords["class"] = TokenType::CLASS;
    keywords["else"] = TokenType::ELSE;
    keywords["false"] = TokenType::FALSE_T;
    keywords["for"] = TokenType::FOR;
    keywords["fun"] = TokenType::FUN;
    keywords["if"] = TokenType::IF;
    keywords["nil"] = TokenType::NIL;
    keywords["or"] = TokenType::OR;
    keywords["print"] = TokenType::PRINT;
    keywords["return"] = TokenType::RETURN;
    keywords["super"] = TokenType::SUPER;
    keywords["this"] = TokenType::THIS;
    keywords["true"] = TokenType::TRUE_T;
    keywords["var"] = TokenType::VAR;
    keywords["while"] = TokenType::WHILE;
}

vector < Token > Scannar::ScanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(EOF_T, "EOF_T", Bundle(), line));
    return move(tokens);
}

bool Scannar::isAtEnd() {
    return current >= source.size();
}

void Scannar::scanToken() {
    char c = advance();
    switch (c) {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '.':
        addToken(TokenType::DOT);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/')) {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        } else {
            addToken(TokenType::SLASH);
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
    case '"':
        convert_to_string();
        break;
    default:
        if (isDigit(c)) {
            handle_number();
        } else if (isAlpha(c)) {
            identifier();
        } else {
            error(line, "Unexpected character.");
        }
    }
}

// get the current char and move forward
char Scannar::advance() {
    return source[current++];
}

// check whether the current char (called by prev char) equal to expected char
// only consume the current character if it’s what we’re looking for
bool Scannar::match(char expected) {
    if (isAtEnd()) {
        return false;
    }
    if (source[current] != expected) {
        return false;
    }
    current++;
    return true;
}

// only get the current char
char Scannar::peek() {
    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

void Scannar::convert_to_string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
        } // from book, support multiline string because it is more difficult to prohibit
        advance();
    }

    if (isAtEnd()) {
        error(line, "Unterminated string.");
        return;
    }
    // The closing ".
    advance();

    // Trim the surrounding quotes.
    string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, Bundle(Bundle_type::STRING, value));
}

bool Scannar::isDigit(char c) {
    return c >= '0' && c <= '9';
}

void Scannar::handle_number() {
    while (isDigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }
    double num = stod(source.substr(start, current - start));
    addToken(TokenType::NUMBER, Bundle(Bundle_type::NUM, "", num)); // we don't parse str to double now, we can use stod later
}

char Scannar::peekNext() {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

void Scannar::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }
    string text = source.substr(start, current - start);
    auto type_itr = keywords.find(text);
    if (type_itr == keywords.end()) {
        addToken(TokenType::IDENTIFIER);
    } else {
        addToken(type_itr -> second);
    }
}

bool Scannar::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

bool Scannar::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

// add token only with type(1 char token)
void Scannar::addToken(TokenType type) {
    addToken(type, Bundle());
}

// lexeme is the string representation in source, so we can directly print it
void Scannar::addToken(TokenType type, Bundle literial) {
    tokens.push_back(Token(type, source.substr(start, current - start), literial, line));
}

string AstPrinter::print(Expr * expr) {
    return expr -> accept(this).str;
}

Bundle AstPrinter::visitBinaryExpr(Binary * expr) {
    string ret = parenthesize(expr -> optor.lexeme, {expr -> left, expr -> right});
    // return ret;
    return Bundle(Bundle_type::STRING, ret);
}

Bundle AstPrinter::visitGroupingExpr(Grouping * expr) {
    string ret = parenthesize("group", {expr -> expression});
    // return ret;
    return Bundle(Bundle_type::STRING, ret);
}

Bundle AstPrinter::visitLiteralExpr(Literal * expr) {
    /*
    if (expr->value.type == Bundle_type::NIL) { return Bundle(Bundle_type::NIL); }
    Bundle_type ret_type = expr->value.type;
    strgin ret = expr->value.str;
    if (ret_type == Bundle_type::STRING)
    return Bundle(Bundle_type::STRING, ret);
    */
    string ret = expr -> value.toString();
    return Bundle(Bundle_type::STRING, ret);
}


Bundle AstPrinter::visitUnaryExpr(Unary * expr) {
    string ret = parenthesize(expr -> optor.lexeme, {expr -> right});
    // return ret;
    return Bundle(Bundle_type::STRING, ret);
}

string AstPrinter::parenthesize(string name, vector < Expr * > exprs) {
    string builder = "(" + name;
    for (Expr * expr: exprs) {
        builder += " ";
        builder += expr -> accept(this).toString();
    }
    builder += ")";
    return builder;
}

Bundle Interpreter::visitLiteralExpr(Literal * expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitLiteralExpr\n"; }
    return expr -> value;
}

Bundle Interpreter::visitLogicalExpr(Logical* expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitLogicalExpr\n"; }

    Bundle left = evaluate(expr->left);
    if (expr->optor.type == TokenType::OR) {
        if (isTruthy(left)) { return left; }
    } else {
        if (!isTruthy(left)) { return left; }
    }
    return evaluate(expr->right);
}

Bundle Interpreter::visitCallExpr(Call* expr) {
    Bundle callee = evaluate(expr->callee);
    vector<Bundle> arguments;
    for (Expr* argument: expr->arguments) {
        arguments.push_back(evaluate(argument));
    }
    
    
}

Bundle Interpreter::visitGroupingExpr(Grouping * expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitGroupingExpr\n"; }

    return evaluate(expr -> expression);
}

Interpreter::Interpreter() {
    environment = new Environment();
}


Bundle Interpreter::evaluate(Expr * expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::evaluate\n"; }

    return expr -> accept(this);
}

void Interpreter::execute(Stmt* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::evaluate\n"; }

    stmt->accept(this);
}

void Interpreter::executeBlock(vector<Stmt*> statements, Environment* b_environment) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::executeBlock\n"; }

    Environment* previous = this->environment;
    try {
        this->environment = b_environment;
        for (Stmt* statement: statements) {
            execute(statement);
        }
    } catch (...) { // catch any exception https://stackoverflow.com/questions/315948/c-catching-all-exceptions
        this->environment = previous;
        cout << "execute(statement) throw exception\n";
    }
    this->environment = previous;
}

Bundle Interpreter::visitExpressionStmt(Expression* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitExpressionStmt\n"; }

    evaluate(stmt->expression);
    return Bundle();
}

Bundle Interpreter::visitPrintStmt(Print* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitPrintStmt\n"; }
    Bundle value = evaluate(stmt->expression);
    cout << value.toString() << '\n'; // for every statement we add new line
    return Bundle();
}

Bundle Interpreter::visitVarStmt(Var* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitVarStmt\n"; }

    Bundle value;
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }
    environment->define(stmt->name.lexeme, value);
    return Bundle();
}

Bundle Interpreter::visitWhileStmt(While* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitWhileStmt\n"; }

    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
    return Bundle();
}

Bundle Interpreter::visitBlockStmt(Block* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitBlockStmt\n"; }

    executeBlock(stmt->statements, new Environment(environment)); // copy constructor?
    return Bundle();
}

Bundle Interpreter::visitIfStmt(If* stmt) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitIfStmt\n"; }

    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr){
        execute(stmt->elseBranch);
    }
    return Bundle();
}

Bundle Interpreter::visitAssignExpr(Assign* expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitAssignExpr\n"; }

    Bundle value = evaluate(expr->value);
    environment->assign(expr->name, value);
    return value;
}


/*
void Interpreter::interpret(Expr * expr) {
    try {
        Bundle value = evaluate(expr);
        cout << value.toString() << '\n';
    } catch (RuntimeError & err) {
        runtimeError(err);
    }
}
*/

void Interpreter::interpret(vector<Stmt*> statements) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::interpret\n"; }

    try {
      for (Stmt* statement : statements) {
        execute(statement);
      }
    } catch (RuntimeError& err) {
      runtimeError(err);
    }
  }

Bundle Interpreter::visitBinaryExpr(Binary * expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitBinaryExpr\n"; }

    Bundle left = evaluate(expr -> left);
    Bundle right = evaluate(expr -> right);
    bool b_ret = false;
    double d_ret = 0;
    string s_ret = "";
    //cout << "expr->optor.type = " << expr -> optor.type << '\n';
    //cout << "left is string? " << (left.type == Bundle_type::STRING) << '\n';
    //cout << "right is STRING? " << (left.type == Bundle_type::STRING) << '\n';
    switch (expr -> optor.type) {
    case TokenType::GREATER:
        checkNumberOperands(expr -> optor, left, right);
        b_ret = left.num > right.num;
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr -> optor, left, right);
        b_ret = left.num >= right.num;
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    case TokenType::LESS:
        checkNumberOperands(expr -> optor, left, right);
        b_ret = left.num < right.num;
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(expr -> optor, left, right);
        b_ret = left.num <= right.num;
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    case TokenType::MINUS:
        checkNumberOperands(expr -> optor, left, right);
        d_ret = left.num - right.num;
        return Bundle(Bundle_type::NUM, "", d_ret);
    case TokenType::PLUS:
        if ((left.type == Bundle_type::NUM) && (right.type == Bundle_type::NUM)) {
            d_ret = left.num + right.num;
            return Bundle(Bundle_type::NUM, "", d_ret);
        }
        if ((left.type == Bundle_type::STRING) && (right.type == Bundle_type::STRING)) {
            s_ret = left.str + right.str;
            return Bundle(Bundle_type::STRING, s_ret);
        }
        throw RuntimeError(expr -> optor, "Operands must be two numbers or two strings.");
        break;
    case TokenType::SLASH:
        checkNumberOperands(expr -> optor, left, right);
        d_ret = left.num / right.num;
        return Bundle(Bundle_type::NUM, "", d_ret);
    case TokenType::STAR:
        checkNumberOperands(expr -> optor, left, right);
        d_ret = left.num * right.num;
        return Bundle(Bundle_type::NUM, "", d_ret);
    case TokenType::BANG_EQUAL:
        b_ret = !isEqual(left, right);
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    case TokenType::EQUAL_EQUAL:
        b_ret = isEqual(left, right);
        return Bundle(Bundle_type::BOOL, "", 0, b_ret);
    }
    return Bundle(Bundle_type::NIL);
}

Bundle Interpreter::visitUnaryExpr(Unary * expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitUnaryExpr\n"; }

    Bundle right = evaluate(expr -> right);
    switch (expr -> optor.type) {
    case TokenType::BANG:
        return Bundle(Bundle_type::BOOL, "", 0, !isTruthy(right));
    case TokenType::MINUS:
        checkNumberOperand(expr -> optor, right);
        right.num = -right.num;
        return Bundle(Bundle_type::NUM, "", right.num);
    }
    return Bundle();
}

Bundle Interpreter::visitVariableExpr(Variable* expr) {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::visitVariableExpr\n"; }

    return environment->get(expr->name);    
}

void Interpreter::checkNumberOperand(Token optor, Bundle operand) { // TODO
    if (operand.type == Bundle_type::NUM) {
        return;
    }
    throw RuntimeError(optor, "Operand must be a number.");
}
void Interpreter::checkNumberOperands(Token optor, Bundle left, Bundle right) { // TODO
    if (INTERPRETER_TRACE) { cout << "called Interpreter::checkNumberOperands\n"; }

    if ((left.type == Bundle_type::NUM) && (right.type == Bundle_type::NUM)) {
        return;
    }
    throw RuntimeError(optor, "Operands must be numbers.");
}
bool Interpreter::isTruthy(Bundle obj) const {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::isTruthy\n"; }

    // cout << "called Interpreter::isTruthy()\n";
    if (obj.type == Bundle_type::NIL) {
        return false;
    } else if (obj.type == Bundle_type::BOOL) {
        return obj.boo;
    }
    return true;
}

bool Interpreter::isEqual(Bundle a, Bundle b) const {
    if (INTERPRETER_TRACE) { cout << "called Interpreter::isEqual\n"; }

    if ((a.type == Bundle_type::NIL) && (b.type == Bundle_type::NIL)) {
        return true;
    }
    if (a.type == Bundle_type::NIL) {
        return false;
    }
    return a.isEqual(b);
}

Binary::Binary(Expr * left, Token optor, Expr * right): left {left}, optor {optor}, right {right} {}

Bundle Binary::accept(Visitor * visitor) {
    return visitor -> visitBinaryExpr(this);
}

Grouping::Grouping(Expr * expression) {
    this -> expression = expression;
}

Bundle Grouping::accept(Visitor * visitor) {
    return visitor -> visitGroupingExpr(this);
}

Literal::Literal(Bundle value) {
    this -> value = value;
}

Bundle Literal::accept(Visitor * visitor) {
    return visitor -> visitLiteralExpr(this);
}

Logical::Logical(Expr* left, Token optor, Expr* right):left(left), optor(optor), right(right) {}

Bundle Logical::accept(Visitor *visitor) {
    return visitor->visitLogicalExpr(this);
}

Unary::Unary(Token optor, Expr * right): optor {optor}, right {right} {}

Bundle Unary::accept(Visitor * visitor) {
    return visitor -> visitUnaryExpr(this);
}

Variable::Variable(Token name):name(name) {}

Bundle Variable::accept(Visitor* visitor) {
    return visitor->visitVariableExpr(this);    
}

Assign::Assign(Token name, Expr* value):name(name), value(value) {}

Bundle Assign::accept(Visitor* visitor) {
    return visitor->visitAssignExpr(this);
}

Call::Call(Expr* callee, Token paren, vector<Expr*> argument):callee(callee),
    paren(paren), argument(argument) {}
    
Bundle Call::accept::accept(Visitor* visitor) {
    return visitCallExpr(this);
}

Block::Block(vector<Stmt*> statements):statements(statements) {}

Bundle Block::accept(stmt_Visitor* visitor) {
    return visitor->visitBlockStmt(this);
}

Expression::Expression(Expr* expression):expression(expression) {}

Bundle Expression::accept(stmt_Visitor* visitor) {
    return visitor->visitExpressionStmt(this);
}

Print::Print(Expr* expression):expression(expression) {}

Bundle Print::accept(stmt_Visitor* visitor) {
    return visitor->visitPrintStmt(this);
}

Var::Var(Token name, Expr* initializer):name(name), initializer(initializer) {}

Bundle Var::accept(stmt_Visitor* visitor) {
    return visitor->visitVarStmt(this);
}

If::If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch):condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

Bundle If::accept(stmt_Visitor* visitor) {
    return visitor->visitIfStmt(this);
}

While::While(Expr* condition, Stmt* body):condition(condition), body(body) {}

Bundle While::accept(stmt_Visitor* visitor) {
    return visitor->visitWhileStmt(this);
}

Parser::Parser(vector < Token > tokens) {
    this -> tokens = tokens;
    current = 0;
}

Expr * Parser::expression() {
    return assignment();
}

Stmt* Parser::declaration() {
    try {
        if (match({TokenType::VAR})) {
            return varDeclaration();
        }
        return statement();
    } catch (ParseError& err) {
        synchronize();
        return nullptr;
    }
}

Stmt* Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    Expr* initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return new Var(name, initializer);
}

Stmt* Parser::whileStatement() {
    // cout << "called Parser::whileStatement()\n";
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    Stmt* body = statement();

    return new While(condition, body);
}

Stmt* Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition."); 
    Stmt* thenBranch = statement();
    Stmt* elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
      elseBranch = statement();
    }
    return new If(condition, thenBranch, elseBranch);
}

Stmt* Parser::statement() {
    if (match({TokenType::FOR})) { return forStatement(); }
    if (match({TokenType::IF})) { return ifStatement(); }
    if (match({TokenType::PRINT})) { return printStatement(); }
    if (match({TokenType::WHILE})) { return whileStatement(); }
    if (match({TokenType::LEFT_BRACE})) { return new Block(block()); }
    return expressionStatement();
}

Stmt* Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    Stmt* initializer;
    if (match({TokenType::SEMICOLON})) {
      initializer = nullptr;
    } else if (match({TokenType::VAR})) {
      initializer = varDeclaration();
    } else {
      initializer = expressionStatement();
    }
    
    Expr* condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
      condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    
    Expr* increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
      increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    Stmt* body = statement();
    
    if (increment != nullptr) {
      body = new Block( {body, new Expression(increment)} );
    }
    
    if (condition == nullptr) { 
        condition = new Literal(Bundle(Bundle_type::BOOL, "", 0, true)); 
    }
    body = new While(condition, body);
    if (initializer != nullptr) {
      body = new Block({initializer, body});
    }
    return body;
}
  
Stmt* Parser::printStatement() {
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return new Print(value);
}

Stmt* Parser::expressionStatement() {
    Expr* expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return new Expression(expr);
}

vector<Stmt*> Parser::block() {
    vector<Stmt*> statements;
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

Expr* Parser::assignment() {
    Expr* expr = Or();
    
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        Expr* value = assignment();
        
        Variable* variable = dynamic_cast<Variable*>(expr);
        if (variable != nullptr) {
            Token name = variable->name;
            return new Assign(name, value);
        }
        
        error(equals, "Invalid assignment target.");
    }
    return expr;
}


  
Expr* Parser::Or() {
    Expr* expr = And();
    while (match({TokenType::OR})) {
        Token optor = previous();
        Expr* right = And();
        expr = new Logical(expr, optor, right);
    }
    return expr;
}

Expr* Parser::And() {
    Expr* expr = equality();
    while (match({TokenType::AND})) {
        Token optor = previous();
        Expr* right = equality();
        expr = new Logical(expr, optor, right);
    }
    return expr;
}

Expr * Parser::equality() {
    // cout << "called Parser::equality()\n";
    Expr * expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token optor = previous();
        Expr * right = comparison();
        expr = new Binary(expr, optor, right);
    }
    return expr;
}

bool Parser::match(vector < TokenType > types) {
    //cout << "call match()\n";
    for (TokenType type: types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_T;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Expr * Parser::comparison() {
    // cout << "called Parser::comparison()\n";
    Expr * expr = term();
    while (match({
            TokenType::GREATER,
            TokenType::GREATER_EQUAL,
            TokenType::LESS,
            TokenType::LESS_EQUAL})) {
        Token optor = previous();
        Expr * right = term();
        expr = new Binary(expr, optor, right);
    }
    return expr;
}

Expr * Parser::term() {
    //cout << "called Parser::term()\n";
    Expr * expr = factor();
    //cout << "after called factor()\n";
    while (match({
            TokenType::MINUS,
            TokenType::PLUS
        })) {
        Token optor = previous();
        Expr * right = factor();
        expr = new Binary(expr, optor, right);
    }
    return expr;
}

Expr * Parser::factor() {
    //cout << "called factor()\n";
    Expr * expr = unary();
    //cout << "after called unary()\n";
    while (match({
            TokenType::SLASH,
            TokenType::STAR
        })) {
        Token optor = previous();
        Expr * right = unary();
        expr = new Binary(expr, optor, right);
    }
    return expr;
}

Expr * Parser::unary() {
    //cout << "called unary()\n";
    if (match({
            TokenType::BANG,
            TokenType::MINUS
        })) {
        Token optor = previous();
        Expr * right = unary();
        return new Unary(optor, right);
    }
    //Expr * p = primary();
    //cout << "unary() before return primary()\n";
    return call();
}

Expr* Parser::call() {
    Expr* expr = primary();

    while (true) { 
      if (match({TokenType::LEFT_PAREN})) {
        expr = finishCall(expr);
      } else {
        break;
      }
    }

    return expr;
}

Expr* Parser::finishCall(Expr* callee) {
    vector<Expr*> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN,
                          "Expect ')' after arguments.");

    return new Call(callee, paren, arguments);
}
  
Expr * Parser::primary() {
    //cout << "called primary()\n";
    if (match({TokenType::FALSE_T})) {
        return new Literal(Bundle(Bundle_type::BOOL, "", 0, false));
    }
    if (match({TokenType::TRUE_T})) {
        return new Literal(Bundle(Bundle_type::BOOL, "", 0, true));
    }
    if (match({TokenType::NIL})) {
        return new Literal(Bundle(Bundle_type::NIL));
    }

    if (match({TokenType::NUMBER})) {
        //cout << "primary is num\n";
        return new Literal(Bundle(Bundle_type::NUM, "", previous().literal.num));
    }
    if (match({TokenType::STRING})) {
        //cout << "primary is string\n";
        return new Literal(Bundle(Bundle_type::STRING, previous().literal.str));
    }
    
    if (match({TokenType::IDENTIFIER})) {
      return new Variable(previous());
    }
    
    //cout << "primary is NOT string or num\n";
    if (match({TokenType::LEFT_PAREN})) {
        Expr * expr = expression();
        consume(TokenType::RIGHT_PAREN, string("Expect ')' after expression."));
        return new Grouping(expr);
    }
    throw error(peek(), "Expect expression.");
}

/*
Expr * Parser::parse() {
    try {
        return expression();
    } catch (ParseError & error) {
        return nullptr;
    }
}
*/

vector<Stmt*> Parser::parse() {
    vector<Stmt*> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}


Token Parser::consume(TokenType type, string message) {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

ParseError Parser::error(Token token, string message) {
    error(token, message);
    return ParseError();
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        }

        advance();
    }
}

Bundle::Bundle(Bundle_type type, string str, double num, bool boo): type(type), str(str), num(num), boo(boo) {}

string Bundle::toString() const {
    if (type == Bundle_type::STRING) {
        return str; // or " str " ?
    } else if (type == Bundle_type::NUM) {
        return to_string(num);
    } else if (type == Bundle_type::BOOL) {
        if (boo) {
            return "true";
        }
        return "false";
    } else if (type == Bundle_type::NIL) {
        return "Nil";
    }
    return "void";
}

// different type = false, value type check value, NIL,VOID = true;
bool Bundle::isEqual(Bundle other) const {
    if (type != other.type) {
        return false;
    }
    // equal type now
    if (type == Bundle_type::STRING) {
        return str == other.str;
    }
    if (type == Bundle_type::NUM) {
        return num == other.num;
    }
    if (type == Bundle_type::BOOL) {
        return boo == other.boo;
    }
    return true;
}

RuntimeError::RuntimeError(Token token, string message): token(token), message(message) {}

void Environment::define(string name, Bundle value) {
    values[name] = value;
}

Bundle Environment::get(Token name) {
    if (values.count(name.lexeme)) {
        // cout << "called Environment::get, hit, name = " << name << '\n';
        return values[name.lexeme];
    }
    if (enclosing != nullptr) {
        // cout << "called Environment::get, not in current scope, name = " << name << '\n';
        return enclosing->get(name);    
    }
    
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(Token name, Bundle value) {
    // cout << "called Environment::assign\n";
    //cout << name.lexeme << " " << value.toString() << '\n';
    if (values.count(name.lexeme)) {
        // cout << "variable exist and change successfully, eid = " << eid << "\n";
        // check_values(this);
        values[name.lexeme] = value;
        // cout << "values[name.lexeme] = " << values[name.lexeme].toString() << '\n';
        // check_values(this);
        return;
    }
    if (enclosing != nullptr) {
        // cout << "variable not in current scope, find outer\n";
        enclosing->assign(name, value);
        // cout << "after enclosing assign, check enclosing: " << enclosing->values.count(name.lexeme) << '\n';
        // cout << "enclosing 's num = " << enclosing->values[name.lexeme].toString() << '\n';
        // cout << "enclosing eid = " << enclosing->eid << '\n';
        // check_values(enclosing);
        return;    
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

Environment::Environment(Environment* enclosing):enclosing(enclosing) {
    if (enclosing != nullptr) {
        eid = enclosing->eid;
    } else {
        eid = 0;
    }
}
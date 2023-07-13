#include "Stmt.h"

Stmt::Block::Block(std::vector<Stmt*> statements): statements {statements} {}

Object Stmt::Block::accept(Visitor* visitor) {
	return visitor->visit_Block_Stmt(this);
};

Stmt::Expression::Expression(Expr* expression): expression {expression} {}

Object Stmt::Expression::accept(Visitor* visitor) {
	return visitor->visit_Expression_Stmt(this);
};

Stmt::Function::Function(Token name, std::vector<Token> params, std::vector<Stmt*> body): name {name}, params {params}, body {body} {}

Object Stmt::Function::accept(Visitor* visitor) {
	return visitor->visit_Function_Stmt(this);
};

Stmt::If::If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch): condition {condition}, thenBranch {thenBranch}, elseBranch {elseBranch} {}

Object Stmt::If::accept(Visitor* visitor) {
	return visitor->visit_If_Stmt(this);
};

Stmt::Print::Print(Expr* expression): expression {expression} {}

Object Stmt::Print::accept(Visitor* visitor) {
	return visitor->visit_Print_Stmt(this);
};

Stmt::Return::Return(Token keyword, Expr* value): keyword {keyword}, value {value} {}

Object Stmt::Return::accept(Visitor* visitor) {
	return visitor->visit_Return_Stmt(this);
};

Stmt::Var::Var(Token name, Expr* initializer): name {name}, initializer {initializer} {}

Object Stmt::Var::accept(Visitor* visitor) {
	return visitor->visit_Var_Stmt(this);
};

Stmt::While::While(Expr* condition, Stmt* body): condition {condition}, body {body} {}

Object Stmt::While::accept(Visitor* visitor) {
	return visitor->visit_While_Stmt(this);
};


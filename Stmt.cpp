#include "Stmt.h"

Stmt::Block::Block(std::vector<std::unique_ptr<Stmt>> statements): statements {std::move(statements)} {}

Object Stmt::Block::accept(Visitor* visitor) {
	return visitor->visit_Block_Stmt(this);
};

Stmt::Expression::Expression(std::unique_ptr<Expr> expression): expression {std::move(expression)} {}

Object Stmt::Expression::accept(Visitor* visitor) {
	return visitor->visit_Expression_Stmt(this);
};

Stmt::Function::Function(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body): name {name}, params {std::move(params)}, body {std::move(body)} {}

Object Stmt::Function::accept(Visitor* visitor) {
	return visitor->visit_Function_Stmt(this);
};

Stmt::If::If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch): condition {std::move(condition)}, thenBranch {std::move(thenBranch)}, elseBranch {std::move(elseBranch)} {}

Object Stmt::If::accept(Visitor* visitor) {
	return visitor->visit_If_Stmt(this);
};

Stmt::Print::Print(std::unique_ptr<Expr> expression): expression {std::move(expression)} {}

Object Stmt::Print::accept(Visitor* visitor) {
	return visitor->visit_Print_Stmt(this);
};

Stmt::Return::Return(Token keyword, std::unique_ptr<Expr> value): keyword {keyword}, value {std::move(value)} {}

Object Stmt::Return::accept(Visitor* visitor) {
	return visitor->visit_Return_Stmt(this);
};

Stmt::Var::Var(Token name, std::unique_ptr<Expr> initializer): name {name}, initializer {std::move(initializer)} {}

Object Stmt::Var::accept(Visitor* visitor) {
	return visitor->visit_Var_Stmt(this);
};

Stmt::While::While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body): condition {std::move(condition)}, body {std::move(body)} {}

Object Stmt::While::accept(Visitor* visitor) {
	return visitor->visit_While_Stmt(this);
};


#include "Expr.h"

Expr::Assign::Assign(Token name, Expr* value): name {name}, value {value} {}

Object Expr::Assign::accept(Visitor* visitor) {
	return visitor->visit_Assign_Expr(this);
};

Expr::Binary::Binary(Expr* left, Token op, Expr* right): left {left}, op {op}, right {right} {}

Object Expr::Binary::accept(Visitor* visitor) {
	return visitor->visit_Binary_Expr(this);
};

Expr::Call::Call(Expr* callee, Token paren, std::vector<Expr*> arguments): callee {callee}, paren {paren}, arguments {arguments} {}

Object Expr::Call::accept(Visitor* visitor) {
	return visitor->visit_Call_Expr(this);
};

Expr::Grouping::Grouping(Expr* expression): expression {expression} {}

Object Expr::Grouping::accept(Visitor* visitor) {
	return visitor->visit_Grouping_Expr(this);
};

Expr::Literal::Literal(Object value): value {value} {}

Object Expr::Literal::accept(Visitor* visitor) {
	return visitor->visit_Literal_Expr(this);
};

Expr::Logical::Logical(Expr* left, Token op, Expr* right): left {left}, op {op}, right {right} {}

Object Expr::Logical::accept(Visitor* visitor) {
	return visitor->visit_Logical_Expr(this);
};

Expr::Unary::Unary(Token op, Expr* right): op {op}, right {right} {}

Object Expr::Unary::accept(Visitor* visitor) {
	return visitor->visit_Unary_Expr(this);
};

Expr::Variable::Variable(Token name): name {name} {}

Object Expr::Variable::accept(Visitor* visitor) {
	return visitor->visit_Variable_Expr(this);
};


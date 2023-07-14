#include "Expr.h"

Expr::Assign::Assign(Token name, std::unique_ptr<Expr> value): name {name}, value {std::move(value)} {}

Object Expr::Assign::accept(Visitor* visitor) {
	return visitor->visit_Assign_Expr(this);
};

Expr::Binary::Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right): left {std::move(left)}, op {op}, right {std::move(right)} {}

Object Expr::Binary::accept(Visitor* visitor) {
	return visitor->visit_Binary_Expr(this);
};

Expr::Call::Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments): callee {std::move(callee)}, paren {paren}, arguments {std::move(arguments)} {}

Object Expr::Call::accept(Visitor* visitor) {
	return visitor->visit_Call_Expr(this);
};

Expr::Grouping::Grouping(std::unique_ptr<Expr> expression): expression {std::move(expression)} {}

Object Expr::Grouping::accept(Visitor* visitor) {
	return visitor->visit_Grouping_Expr(this);
};

Expr::Literal::Literal(Object value): value {std::move(value)} {}

Object Expr::Literal::accept(Visitor* visitor) {
	return visitor->visit_Literal_Expr(this);
};

Expr::Logical::Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right): left {std::move(left)}, op {op}, right {std::move(right)} {}

Object Expr::Logical::accept(Visitor* visitor) {
	return visitor->visit_Logical_Expr(this);
};

Expr::Unary::Unary(Token op, std::unique_ptr<Expr> right): op {op}, right {std::move(right)} {}

Object Expr::Unary::accept(Visitor* visitor) {
	return visitor->visit_Unary_Expr(this);
};

Expr::Variable::Variable(Token name): name {name} {}

Object Expr::Variable::accept(Visitor* visitor) {
	return visitor->visit_Variable_Expr(this);
};


#ifndef Expr_H
#define Expr_H

#include <vector>
#include <memory>
#include "Token.h"
#include "Object.h"
class Expr {
public:
	class Assign;
	class Binary;
	class Call;
	class Grouping;
	class Literal;
	class Logical;
	class Unary;
	class Variable;

	class Visitor;

	virtual Object accept(Visitor* visitor) = 0;
};

class Expr::Visitor {
public:
	virtual Object visit_Assign_Expr(Assign* expr)=0;
	virtual Object visit_Binary_Expr(Binary* expr)=0;
	virtual Object visit_Call_Expr(Call* expr)=0;
	virtual Object visit_Grouping_Expr(Grouping* expr)=0;
	virtual Object visit_Literal_Expr(Literal* expr)=0;
	virtual Object visit_Logical_Expr(Logical* expr)=0;
	virtual Object visit_Unary_Expr(Unary* expr)=0;
	virtual Object visit_Variable_Expr(Variable* expr)=0;
};

class Expr::Assign final: public Expr {
public:
	Token name;
	std::unique_ptr<Expr> value;

	Assign(Token name, std::unique_ptr<Expr> value);
	Object accept(Visitor* visitor) override;
};

class Expr::Binary final: public Expr {
public:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right);
	Object accept(Visitor* visitor) override;
};

class Expr::Call final: public Expr {
public:
	std::unique_ptr<Expr> callee;
	Token paren;
	std::vector<std::unique_ptr<Expr>> arguments;

	Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments);
	Object accept(Visitor* visitor) override;
};

class Expr::Grouping final: public Expr {
public:
	std::unique_ptr<Expr> expression;

	Grouping(std::unique_ptr<Expr> expression);
	Object accept(Visitor* visitor) override;
};

class Expr::Literal final: public Expr {
public:
	Object value;

	Literal(Object value);
	Object accept(Visitor* visitor) override;
};

class Expr::Logical final: public Expr {
public:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right);
	Object accept(Visitor* visitor) override;
};

class Expr::Unary final: public Expr {
public:
	Token op;
	std::unique_ptr<Expr> right;

	Unary(Token op, std::unique_ptr<Expr> right);
	Object accept(Visitor* visitor) override;
};

class Expr::Variable final: public Expr {
public:
	Token name;

	Variable(Token name);
	Object accept(Visitor* visitor) override;
};

#endif

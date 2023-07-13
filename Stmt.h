#ifndef Stmt_H
#define Stmt_H

#include <vector>
#include "Token.h"
#include "Object.h"
#include "Expr.h"

class Stmt {
public:
	class Block;
	class Expression;
	class Function;
	class If;
	class Print;
	class Return;
	class Var;
	class While;

	class Visitor;

	virtual Object accept(Visitor* visitor) = 0;
};

class Stmt::Visitor {
public:
	virtual Object visit_Block_Stmt(Block* expr)=0;
	virtual Object visit_Expression_Stmt(Expression* expr)=0;
	virtual Object visit_Function_Stmt(Function* expr)=0;
	virtual Object visit_If_Stmt(If* expr)=0;
	virtual Object visit_Print_Stmt(Print* expr)=0;
	virtual Object visit_Return_Stmt(Return* expr)=0;
	virtual Object visit_Var_Stmt(Var* expr)=0;
	virtual Object visit_While_Stmt(While* expr)=0;
};

class Stmt::Block final: public Stmt {
public:
	std::vector<Stmt*> statements;

	Block(std::vector<Stmt*> statements);
	Object accept(Visitor* visitor) override;
};

class Stmt::Expression final: public Stmt {
public:
	Expr* expression;

	Expression(Expr* expression);
	Object accept(Visitor* visitor) override;
};

class Stmt::Function final: public Stmt {
public:
	Token name;
	std::vector<Token> params;
	std::vector<Stmt*> body;

	Function(Token name, std::vector<Token> params, std::vector<Stmt*> body);
	Object accept(Visitor* visitor) override;
};

class Stmt::If final: public Stmt {
public:
	Expr* condition;
	Stmt* thenBranch;
	Stmt* elseBranch;

	If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
	Object accept(Visitor* visitor) override;
};

class Stmt::Print final: public Stmt {
public:
	Expr* expression;

	Print(Expr* expression);
	Object accept(Visitor* visitor) override;
};

class Stmt::Return final: public Stmt {
public:
	Token keyword;
	Expr* value;

	Return(Token keyword, Expr* value);
	Object accept(Visitor* visitor) override;
};

class Stmt::Var final: public Stmt {
public:
	Token name;
	Expr* initializer;

	Var(Token name, Expr* initializer);
	Object accept(Visitor* visitor) override;
};

class Stmt::While final: public Stmt {
public:
	Expr* condition;
	Stmt* body;

	While(Expr* condition, Stmt* body);
	Object accept(Visitor* visitor) override;
};

#endif

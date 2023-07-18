#ifndef Interpreter_H
#define Interpreter_H

#include <vector>
#include "Expr.h"
#include "Stmt.h"
#include "Object.h"
#include "Environment.h"

class Interpreter final: public Expr::Visitor, public Stmt::Visitor {
private:
    Environment* environment;
    std::unordered_map<Expr*, int> locals;

    Object evaluate(Expr* expr);
    void execute(Stmt* stmt);
    // false and nil are falsey, and everything else is truthy
    bool is_truthy(Object object);
    bool is_equal(Object a, Object b);
    void check_number_operand(Token op, Object operand);
    void check_number_operands(Token op, Object left, Object right);
    Object lookup_variable(Token name, Expr* expr);
public:
    // holds a fixed reference to the outermost global environment
    Environment globals;

    Interpreter();
    // void interpret(Expr* expression);
    void interpret(std::vector<std::unique_ptr<Stmt>> statements);
    void execute_block(const std::vector<std::unique_ptr<Stmt>>& statements, Environment* environment);
    void resolve(Expr* expr, int depth);

    Object visit_Literal_Expr(Expr::Literal* expr) override;
    Object visit_Logical_Expr(Expr::Logical* expr) override;
    Object visit_Unary_Expr(Expr::Unary* expr) override;
    Object visit_Variable_Expr(Expr::Variable* expr) override;
    Object visit_Binary_Expr(Expr::Binary* expr) override;
    Object visit_Call_Expr(Expr::Call* expr) override;
    Object visit_Grouping_Expr(Expr::Grouping* expr) override;

    Object visit_Expression_Stmt(Stmt::Expression* stmt) override;
    Object visit_Function_Stmt(Stmt::Function* stmt) override;
    Object visit_If_Stmt(Stmt::If* stmt) override;
    Object visit_Print_Stmt(Stmt::Print* stmt) override;
    Object visit_Return_Stmt(Stmt::Return* stmt) override;
    Object visit_Var_Stmt(Stmt::Var* stmt) override;
    Object visit_While_Stmt(Stmt::While* stmt) override;
    Object visit_Assign_Expr(Expr::Assign* expr) override;
    Object visit_Block_Stmt(Stmt::Block* stmt) override;
};  

#endif
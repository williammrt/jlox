#ifndef Resolver_H
#define Resolver_H

#include "Expr.h"
#include "Stmt.h"
#include "Interpreter.h"

class Resolver  final: public Expr::Visitor, public Stmt::Visitor {
private:
    enum class function_type {
        NONE, FUNCTION
    };
    
    Interpreter& interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes;
    function_type current_function = function_type::NONE;
    

    void resolve(Stmt* stmt);
    void resolve(Expr* expr);
    void resolve_function(Stmt::Function* function, function_type type);
    void begin_scope();
    void end_scope();
    void declare(Token name);
    void define(Token name);
    void resolve_local(Expr* expr, Token name);
public:
    Resolver(Interpreter& interpreter);
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);
    Object visit_Block_Stmt(Stmt::Block* stmt) override;
    Object visit_Expression_Stmt(Stmt::Expression* stmt) override;
    Object visit_Function_Stmt(Stmt::Function* stmt) override;
    Object visit_If_Stmt(Stmt::If* stmt) override;
    Object visit_Print_Stmt(Stmt::Print* stmt) override;
    Object visit_Return_Stmt(Stmt::Return* stmt) override;
    Object visit_While_Stmt(Stmt::While* stmt) override;
    Object visit_Var_Stmt(Stmt::Var* stmt) override;

    Object visit_Assign_Expr(Expr::Assign* expr) override;
    Object visit_Variable_Expr(Expr::Variable* expr) override;
    Object visit_Binary_Expr(Expr::Binary* expr) override;
    Object visit_Call_Expr(Expr::Call* expr) override;
    Object visit_Grouping_Expr(Expr::Grouping* expr) override;
    Object visit_Literal_Expr(Expr::Literal* expr) override;
    Object visit_Logical_Expr(Expr::Logical* expr) override;
    Object visit_Unary_Expr(Expr::Unary* expr) override;  
};

#endif
#include "Resolver.h"
#include "Lox.h"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const std::unique_ptr<Stmt>& statement: statements) {
        resolve(statement.get());
    }
}

void Resolver::resolve(Stmt* stmt) {
    stmt->accept(this);
}

void Resolver::resolve(Expr* expr) {
    expr->accept(this);
}

void Resolver::resolve_function(Stmt::Function* function, function_type type) {
    function_type enclosing_function = current_function;
    current_function = type;
    
    begin_scope();
    for (Token param: function->params) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    end_scope();
    current_function = enclosing_function;
}

void Resolver::begin_scope() {
    scopes.push_back({});
}

void Resolver::end_scope() {
    scopes.pop_back();
}

void Resolver::declare(Token name) {
    if (scopes.empty()) {
        return;
    }
    if (scopes.back().count(name.lexeme)) {
        error(name, "Already a variable with this name in this scope.");
    }
    scopes.back()[name.lexeme] = false;
}

void Resolver::define(Token name) {
    if (scopes.empty()) {
        return;
    }
    scopes.back()[name.lexeme] = true;
}

void Resolver::resolve_local(Expr* expr, Token name) {
    for (int i = scopes.size()-1; i >= 0; i -= 1) {
        if (scopes[i].count(name.lexeme)) {
            interpreter.resolve(expr, scopes.size()-1-i);
            return;
        }
    }
}

Resolver::Resolver(Interpreter& interpreter)
    :interpreter {interpreter} {}

Object Resolver::visit_Block_Stmt(Stmt::Block* stmt) {
    begin_scope();
    resolve(stmt->statements);
    end_scope();
    return {};
}

Object Resolver::visit_Expression_Stmt(Stmt::Expression* stmt) {
    resolve(stmt->expression.get());
    return {};
}

Object Resolver::visit_Function_Stmt(Stmt::Function* stmt) {
    declare(stmt->name);
    define(stmt->name);

    resolve_function(stmt, function_type::FUNCTION);
    return {};
}

Object Resolver::visit_If_Stmt(Stmt::If* stmt) {
    resolve(stmt->condition.get());
    resolve(stmt->thenBranch.get());
    if (stmt->elseBranch) {
        resolve(stmt->elseBranch.get());
    }
    return {};
}

Object Resolver::visit_Print_Stmt(Stmt::Print* stmt) {
    resolve(stmt->expression.get());
    return {};
}

Object Resolver::visit_Return_Stmt(Stmt::Return* stmt) {
    if (current_function == function_type::NONE) {
        error(stmt->keyword, "Can't return from top-level code.");
    }

    if (stmt->value) {
        resolve(stmt->value.get());
    }
    return {};
}

Object Resolver::visit_While_Stmt(Stmt::While* stmt) {
    resolve(stmt->condition.get());
    resolve(stmt->body.get());
    return {};
}

Object Resolver::visit_Var_Stmt(Stmt::Var* stmt) {
    declare(stmt->name);
    if (stmt->initializer) {
        resolve(stmt->initializer.get());
    }
    define(stmt->name);
    return {};
}

Object Resolver::visit_Assign_Expr(Expr::Assign* expr) {
    resolve(expr->value.get());
    resolve_local(expr, expr->name);
    return {};
}

Object Resolver::visit_Variable_Expr(Expr::Variable* expr) {
    if (!scopes.empty() && scopes.back().count(expr->name.lexeme) && scopes.back()[expr->name.lexeme] == false) {
        error(expr->name, "Can't read local variable in its own initializer.");
    }

    resolve_local(expr, expr->name);
    return {};
}

Object Resolver::visit_Binary_Expr(Expr::Binary* expr) {
    resolve(expr->left.get());
    resolve(expr->right.get());
    return {};
}

Object Resolver::visit_Call_Expr(Expr::Call* expr) {
    resolve(expr->callee.get());

    for (const std::unique_ptr<Expr>& argument: expr->arguments) {
        resolve(argument.get());
    }

    return {};
}

Object Resolver::visit_Grouping_Expr(Expr::Grouping* expr) {
    resolve(expr->expression.get());
    return {};
}

Object Resolver::visit_Literal_Expr(Expr::Literal* expr) {
    return {};
}

Object Resolver::visit_Logical_Expr(Expr::Logical* expr) {
    resolve(expr->left.get());
    resolve(expr->right.get());
    return {};
}

Object Resolver::visit_Unary_Expr(Expr::Unary* expr) {
    resolve(expr->right.get());
    return {};
}


#include <stdexcept>
#include <iostream>
#include "Lox.h"
#include "Interpreter.h"
#include "Token.h"
#include "Runtime_Error.h"
#include "Object.h"
#include "Lox_Callable.h"
#include "Return.h"

Object Interpreter::evaluate(Expr* expr) {
    return expr->accept(this);
}

void Interpreter::execute(Stmt* stmt) {
    stmt->accept(this);
}

bool Interpreter::is_truthy(Object object) {
    if (object.index() == 1) { // Nil
        return false;
    }
    if (object.index() == 4) {
        return std::get<bool>(object);
    }
    return true;
}

bool Interpreter::is_equal(Object a, Object b) {
    // type different
    if (a.index() != b.index()) {
        return false;
    }

    switch (a.index()) {    
        case 1:
            return true;
        case 2:
            // notice that in book, it use Java's isequal, 
            // Nan isequal(Nan), but not ==
            return std::get<double>(a) == std::get<double>(b);
        case 3:
            return std::get<std::string>(a) == std::get<std::string>(b);
        case 4:
            return std::get<bool>(a) == std::get<bool>(b);
        default:
            break;
    }

    throw std::runtime_error("Interpreter::is_equal() error");
}

void Interpreter::check_number_operand(Token op, Object operand) {
    if (operand.index() == 2) {
        return;
    }

    throw Lox_runtime_error(op, "Operand must be a number.");
}

void Interpreter::check_number_operands(Token op, Object left, Object right) {
    if (left.index() == 2 && right.index() == 2) {
        return;
    }

    throw Lox_runtime_error(op, "Operands must be numbers.");
}

/*
void Interpreter::interpret(Expr* expression) {
    try {
        Object value = evaluate(expression);
        std::cout << stringify_Object(value) << '\n';
    } catch (Lox_runtime_error& error) {
        report_runtime_error(error);
    }
}
*/

Interpreter::Interpreter() {
    environment = &globals;

    globals.define("clock", new native_clock);
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>> statements) {
    try {
        for (const std::unique_ptr<Stmt>& statement: statements) {
            execute(statement.get());
        }
    } catch (Lox_runtime_error& error) {
        report_runtime_error(error);
    }
}

void Interpreter::execute_block(const std::vector<std::unique_ptr<Stmt>>& statements, Environment* new_env) {
    Environment* previous = environment;
    try {
        environment = new_env;

        for (const std::unique_ptr<Stmt>& statement: statements) {
            execute(statement.get());
        }
    } catch (...) {
        environment = previous;
        throw;
    }
    // finally in Java
    environment = previous;
}

Object Interpreter::visit_Literal_Expr(Expr::Literal* expr) {
    // std::cout << "Literal: index: " << (expr->value).index() << '\n';
    return expr->value;
}

Object Interpreter::visit_Logical_Expr(Expr::Logical* expr) {
    Object left = evaluate(expr->left.get());

    if ((expr->op).type == OR) {
        if (is_truthy(left)) {
            return left;
        }
    } else {
        if (!is_truthy(left)) {
            return left;
        }
    }

    return evaluate(expr->right.get());
}

Object Interpreter::visit_Unary_Expr(Expr::Unary* expr) {
    Object right = evaluate(expr->right.get());

    switch ((expr->op).type) {
        case BANG:
            return !is_truthy(right);
        case MINUS:
            check_number_operand(expr->op, right);
            return static_cast<double>(-std::get<double>(right));
        default:
            break;
    }

    // Unreachable.
    return Nil {};
}

Object Interpreter::visit_Call_Expr(Expr::Call* expr) {
    Object callee = evaluate(expr->callee.get());

    std::vector<Object> arguments;
    for (const std::unique_ptr<Expr>& argument: expr->arguments) {
        arguments.push_back(evaluate(argument.get()));
    }

    // ch 10
    if (callee.index() != 5) {
        throw Lox_runtime_error(expr->paren, "Can only call functions and classes.");
    }

    Lox_callable* function = std::get<Lox_callable*>(callee);
    if (static_cast<int>(arguments.size()) != function->arity()) {
      throw Lox_runtime_error(expr->paren, "Expected " +
          std::to_string(function->arity()) + " arguments but got " +
          std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, arguments);
    
}

Object Interpreter::visit_Variable_Expr(Expr::Variable* expr) {
    return environment->get(expr->name);
}

Object Interpreter::visit_Binary_Expr(Expr::Binary* expr) {
    Object left = evaluate(expr->left.get());
    Object right = evaluate(expr->right.get());
    // std::cout << ".index() " << left.index() << " and " << right.index() << "\n";
    // std::cout << std::get<std::string>(left) << " " << expr->op.lexeme << " " << std::get<std::string>(right) << '\n';
    // std::cout << left.index() << " " << expr->op.lexeme << " " << std::get<double>(right) << '\n';
    switch ((expr->op).type) {
        case BANG_EQUAL:
            return !is_equal(left, right);
        case EQUAL_EQUAL:
            return is_equal(left, right);
        case GREATER:
            check_number_operands(expr->op, left, right);
            return std::get<double>(left) > std::get<double>(right);
        case GREATER_EQUAL:
            check_number_operands(expr->op, left, right);
            return std::get<double>(left) >= std::get<double>(right);
        case LESS:
            check_number_operands(expr->op, left, right);
            return std::get<double>(left) < std::get<double>(right);
        case LESS_EQUAL:
            check_number_operands(expr->op, left, right);
            return std::get<double>(left) <= std::get<double>(right);
        case PLUS:
            if (left.index()==2 && right.index()==2) {
                return static_cast<double>(std::get<double>(left) + std::get<double>(right));
            }
            if (left.index()==3 && right.index()==3) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            throw Lox_runtime_error(expr->op, "Operands must be two numbers or two strings.");
        case MINUS:
            check_number_operands(expr->op, left, right);
            return static_cast<double>(std::get<double>(left) - std::get<double>(right));
        case SLASH:
            check_number_operands(expr->op, left, right);
            return static_cast<double>(std::get<double>(left) / std::get<double>(right));
        case STAR:
            check_number_operands(expr->op, left, right);
            return static_cast<double>(std::get<double>(left) * std::get<double>(right));
        default:
            break;
    }

    // Unreachable.
    return Nil {};
}

Object Interpreter::visit_Grouping_Expr(Expr::Grouping* expr) {
    return evaluate(expr->expression.get());
}

Object Interpreter::visit_Expression_Stmt(Stmt::Expression* stmt) {
    evaluate(stmt->expression.get());
    return {};
}

Object Interpreter::visit_Function_Stmt(Stmt::Function* stmt) {
    Lox_function* function = new Lox_function(stmt, environment);
    environment->define((stmt->name).lexeme, function);
    return {};
}

Object Interpreter::visit_If_Stmt(Stmt::If* stmt) {
    if (is_truthy(evaluate(stmt->condition.get()))) {
        execute(stmt->thenBranch.get());
    } else if (stmt->elseBranch) {
        execute(stmt->elseBranch.get());
    }

    return {};
}

Object Interpreter::visit_Print_Stmt(Stmt::Print* stmt) {
    Object value = evaluate(stmt->expression.get());
    std::cout << stringify_Object(value) << '\n';
    return {};
}

Object Interpreter::visit_Return_Stmt(Stmt::Return* stmt) {
    Object value;
    if (stmt->value) {
        value = evaluate(stmt->value.get());
    }   
    throw Return(value);
}

Object Interpreter::visit_Var_Stmt(Stmt::Var* stmt) {
    Object value = Nil{};
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer.get());
    }

    environment->define((stmt->name).lexeme, value);
    return {};
}

Object Interpreter::visit_While_Stmt(Stmt::While* stmt) {
    while (is_truthy(evaluate(stmt->condition.get()))) {
        execute(stmt->body.get());
    }
    return {};
}

Object Interpreter::visit_Assign_Expr(Expr::Assign* expr) {
    Object value = evaluate(expr->value.get());
    environment->assign(expr->name, value);
    return value;
}

Object Interpreter::visit_Block_Stmt(Stmt::Block* stmt) {
    execute_block(stmt->statements, new Environment(environment));
    return {};
}
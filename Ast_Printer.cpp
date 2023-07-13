#include <iostream>
#include "Ast_Printer.h"

using namespace std;

std::string Ast_Printer::parenthesize(std::string name, std::vector<Expr*> exprs) {
    std::string result;
    result += string("(");
    result += name;
    for (Expr* expr: exprs) {
        result += string(" ");
        result += get<string>(expr->accept(this));
    }
    result += string(")");
    return result;
}

std::string Ast_Printer::print(Expr* expr) {
    return get<std::string>(expr->accept(this));
}

Object Ast_Printer::visit_Binary_Expr(Expr::Binary* expr) {
    return parenthesize((expr->op).lexeme, {expr->left, expr->right});
}

Object Ast_Printer::visit_Grouping_Expr(Expr::Grouping* expr) {
    return parenthesize("group", {expr->expression});
}

Object Ast_Printer::visit_Literal_Expr(Expr::Literal* expr) {
    string literal_str;
    Object literal = expr->value;
    switch (literal.index()) {
        case 0:
            literal_str = "void";
            break;
        case 1:
            literal_str = "nil";
            break;
        case 2:
            literal_str = std::to_string(std::get<double>(literal));
            break;
        case 3:
            literal_str = std::get<std::string>(literal);
            break;
        case 4: 
            if (std::get<bool>(literal)) {
                literal_str = "true";
            } else {
                literal_str = "false";
            }
            break;
    }
    return literal_str;
}

Object Ast_Printer::visit_Unary_Expr(Expr::Unary* expr) {
    return parenthesize((expr->op).lexeme, {expr->right});
}

#include <string>
#include <vector>
#include "Expr.h"
#include "Object.h"

class Ast_Printer: public Expr::Visitor {
private: 
    std::string parenthesize(std::string name, std::vector<Expr*> exprs);
public:
    std::string print(Expr* expr);

    Object visit_Binary_Expr(Expr::Binary* expr) override;
    Object visit_Grouping_Expr(Expr::Grouping* expr) override;
    Object visit_Literal_Expr(Expr::Literal* expr) override;
    Object visit_Unary_Expr(Expr::Unary* expr) override;
};
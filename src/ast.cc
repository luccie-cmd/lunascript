#include "ast.hh"

void luna::Ast::print() {
    if (this->get_type() == AstType::ROOT) {
        fmt::print("AST\n");
    }

    for (AstTypes child : this->get_children()) {
        try {
            if (auto expr = std::get_if<Expr*>(&child)) {
                fmt::print("EXPR\n");
                fmt::print("|- TYPE: `{}`\n", static_cast<int>((*expr)->expr_get_type()));
            } else if (auto funcDecl = std::get_if<FuncDecl*>(&child)) {
                // (*funcDecl)->print(); // Assuming you have a print method in FuncDecl
            } else if (auto varAssign = std::get_if<VarAssign*>(&child)) {
                // Handle VarAssign
            } else if (auto varDecl = std::get_if<VarDecl*>(&child)) {
                // Handle VarDecl
            } else {
                fmt::print("Unexpected variant type!\n");
            }
        } catch (const std::exception& ex) {
            fmt::print("Exception: {}\n", ex.what());
            std::exit(1);
        }
    }
}

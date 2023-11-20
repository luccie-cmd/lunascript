#include "ast.hh"

void luna::Ast::print() {
    if (this->get_type() == AstType::ROOT) {
        fmt::print("AST\n");
    }

    for (AstTypes child : this->get_children()) {
        try {
            if (auto expr = std::get_if<Expr*>(&child)) {
                fmt::print("EXPR\n");
                switch(((*expr)->expr_get_type())){
                    case ExprType::NONE: {
                        fmt::print("Bug in parsing step!\n");
                        std::exit(1);
                    } break;
                    case ExprType::CALL: {
                        fmt::print("|- NAME: {}\n", (*expr)->call_get_name());
                    } break;
                    default: {
                        fmt::print("Unhanled ExprType! `{}`\n", static_cast<u64>((*expr)->expr_get_type()));
                        std::exit(1);
                    } break;
                }
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


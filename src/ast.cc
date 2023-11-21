#include "ast.hh"

void luna::Ast::print(std::string prefix, std::string AstName) {
    if (this->get_type() == AstType::ROOT) {
        fmt::print("{}{}\n", prefix, AstName);
    }

    for (AstTypes child : this->get_children()) {
        try {
            if (auto expr = std::get_if<Expr>(&child)) {
                fmt::print("{}|- EXPR\n", prefix);
                switch(expr->get_type()){
                    case ExprType::NONE: {
                        fmt::print("Bug in parsing step!\n");
                        std::exit(1);
                    } break;
                    case ExprType::CALL: {
                        fmt::print("{}|    |- NAME: {}\n", prefix, expr->call_get_name());
                            fmt::print("{}|    |- OPERANDS\n", prefix);
                        for(Token t : expr->call_get_operands()){
                            fmt::print("{}|        |- `{}`\n", prefix, t._value);
                        }
                    } break;
                    default: {
                        fmt::print("Unhanled ExprType! `{}`\n", static_cast<u64>(expr->get_type()));
                        std::exit(1);
                    } break;
                }
            } else if (auto funcDecl = std::get_if<FuncDecl>(&child)) {
                fmt::print("{}|- FUNC DECL\n", prefix);
                fmt::print("{}|    |- NAME: `{}`\n", prefix, funcDecl->get_name());
                Ast body = funcDecl->get_body();
                body.print("    ", "BODY");
            } else if (auto varAssign = std::get_if<VarAssign>(&child)) {
                // Handle VarAssign
            } else if (auto varDecl = std::get_if<VarDecl>(&child)) {
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
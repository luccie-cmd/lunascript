// For any core defined functions (Loc)
#include "core.hh"
#include "ast.hh"

std::string luna::Loc::to_str()
{
    return std::string(fmt::format("{}:{}:{}", _file, _row, _col));
}

void luna::Loc::update(int c)
{
    if (c == '\n')
    {
        _col = 0;
        _row++;
    }
    else if (c == '\t')
    {
        _col += 4;
    }
    else
    {
        _col++;
    }
}

void luna::BlockStmt::print(){
    fmt::print("    |- Block statement\n");
    Ast new_ast;
    for(AstTypes types : _body){
        AstType type = static_cast<AstType>(types.index()+1);
        switch(type){
            case luna::AstType::VAR_DECL: {
                fmt::print("        |- Var Decl: {}\n", std::get<std::shared_ptr<VarDecl>>(types)->get_name());
            } break;
            case luna::AstType::VAR_ASSIGN: {
                fmt::print("        |- Var Assign: {} = {}\n", 
                    std::get<std::shared_ptr<VarAssign>>(types)->get_name(), 
                    std::get<std::shared_ptr<VarAssign>>(types)->get_value());
            } break;
            case luna::AstType::VAR_DECLASSIGN: {
                fmt::print("        |- Var Decl: {}\n", std::get<std::shared_ptr<VarDeclAssign>>(types)->get_name());
                fmt::print("        |- Var Assign: {} = {}\n", 
                    std::get<std::shared_ptr<VarDeclAssign>>(types)->get_name(), 
                    std::get<std::shared_ptr<VarDeclAssign>>(types)->get_value());
            } break;
            case luna::AstType::EXPR: {
                auto expr = std::get<ExprTypes>(types);
                ExprType exprType = static_cast<ExprType>(expr.index());
                switch (exprType) {
                    case ExprType::CALL: {
                        std::shared_ptr<CallExpr> call = std::get<std::shared_ptr<CallExpr>>(expr);
                        fmt::print("        |- Call Expr\n");
                        fmt::print("            |- Name: {}\n", call->get_name());
                        for(Token t : call->get_operands()){
                            fmt::print("            |- Operand: `{}`\n", t._value);
                        }
                    } break;
                    default: {
                        fmt::print("UNREACHABLE EXPRESSION TYPE: {}\n", (int)exprType);
                    } break;
                }
            } break;
            case luna::AstType::STMT: {
                auto stmt = std::get<StmtTypes>(types);
                StmtType stmtType = static_cast<StmtType>(stmt.index()+1);
                switch (stmtType) {
                    case StmtType::BLOCK:
                    default: {
                        fmt::print("UNREACHABLE STATEMENT TYPE: {}\n", (int)stmtType);
                    } break;
                }
            } break;
            case luna::AstType::FUNC_DECL:
            case luna::AstType::ROOT:
            default: {
                fmt::print("UNREACHABLE AST TYPE: {}\n", (int)type);
            } break;
        }
    }
}

void luna::Ast::print(){
    fmt::print("AST\n");
    for(AstTypes child : children){
        AstType type = static_cast<AstType>(child.index()+1);
        switch(type){
            case luna::AstType::VAR_DECL: {
                if (std::get<std::shared_ptr<VarDecl>>(child)->get_name() == "__end__") {
                    continue;
                }
                fmt::print("|- Var Decl: {}\n", std::get<std::shared_ptr<VarDecl>>(child)->get_name());
            } break;
            case luna::AstType::VAR_ASSIGN: {
                fmt::print("|- Var Assign: {} = {}\n", 
                    std::get<std::shared_ptr<VarAssign>>(child)->get_name(), 
                    std::get<std::shared_ptr<VarAssign>>(child)->get_value());
            } break;
            case luna::AstType::VAR_DECLASSIGN: {
                fmt::print("|- Var Decl: {}\n", std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
                fmt::print("|- Var Assign: {} = {}\n", 
                    std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name(), 
                    std::get<std::shared_ptr<VarDeclAssign>>(child)->get_value());
            } break;
            case luna::AstType::EXPR: {
                auto expr = std::get<ExprTypes>(child);
                ExprType exprType = static_cast<ExprType>(expr.index());
                switch (exprType) {
                    case ExprType::CALL: {
                        std::shared_ptr<CallExpr> call = std::get<std::shared_ptr<CallExpr>>(expr);
                        fmt::print("|- Call Expr\n");
                        fmt::print("    |- Name: {}\n", call->get_name());
                        for(Token t : call->get_operands()){
                            fmt::print("    |- Operand: `{}`\n", t._value);
                        }
                    } break;
                    default: {
                        fmt::print("UNREACHABLE EXPRESSION TYPE: {}\n", (int)exprType);
                    } break;
                }
            } break;
            case luna::AstType::STMT: {
                auto stmt = std::get<StmtTypes>(child);
                StmtType stmtType = static_cast<StmtType>(stmt.index());
                switch (stmtType) {
                    case StmtType::BLOCK:
                    default: {
                        fmt::print("UNREACHABLE STATEMENT TYPE: {}\n", (int)stmtType);
                    } break;
                }
            } break;
            case luna::AstType::FUNC_DECL: {
                auto decl = std::get<std::shared_ptr<FuncDecl>>(child).get();
                fmt::print("|- Func Decl: {}\n", decl->get_name());
                BlockStmt body = decl->get_body();
                body.print();
            } break;
            case luna::AstType::ROOT:
            default: {
                fmt::print("UNREACHABLE AST TYPE: {}\n", (int)type);
            } break;
        }
    }
}

void luna::BlockStmt::populate_curent_scope(){
    for(std::pair<std::string, std::string> var : parent_scope.variables){
        current.variables.insert(var);
    }
    for(std::string func : parent_scope.declared_functions){
        current.declared_functions.push_back(func);
    }
    for(AstTypes child : _body){
        if (std::shared_ptr<VarDecl>* decl = std::get_if<std::shared_ptr<VarDecl>>(&child)) {
            auto deref = *decl;
            current.variables.insert(std::make_pair(deref->get_name(), "0"));
        } else if(std::shared_ptr<VarAssign>* assign = std::get_if<std::shared_ptr<VarAssign>>(&child)){
            auto deref = *assign;
            current.variables.insert_or_assign(deref->get_name(), deref->get_value());
        } else if(std::shared_ptr<VarDeclAssign>* assignDecl = std::get_if<std::shared_ptr<VarDeclAssign>>(&child)){
            auto deref = *assignDecl;
            current.variables.insert(std::make_pair(deref->get_name(), deref->get_value()));
        }
    }
}
void luna::Ast::populate_scope(){
    for(AstTypes child : children){
        if (std::shared_ptr<VarDecl>* decl = std::get_if<std::shared_ptr<VarDecl>>(&child)) {
            auto deref = *decl;
            current.variables.insert(std::make_pair(deref->get_name(), "0"));
        } else if(std::shared_ptr<VarAssign>* assign = std::get_if<std::shared_ptr<VarAssign>>(&child)){
            auto deref = *assign;
            current.variables.insert_or_assign(deref->get_name(), deref->get_value());
        } else if(std::shared_ptr<VarDeclAssign>* assignDecl = std::get_if<std::shared_ptr<VarDeclAssign>>(&child)){
            auto deref = *assignDecl;
            current.variables.insert(std::make_pair(deref->get_name(), deref->get_value()));
        }
    }
}
void luna::Scope::print(){
    fmt::print("Scope\n");
    fmt::print("Variables in current_scope:\n");
    for (const auto& entry : variables) {
        fmt::print("Key: {}, Value: {}\n", entry.first, entry.second);
    }
    fmt::print("End Scope\n");
}
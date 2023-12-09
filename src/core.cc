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

void luna::Ast::print(){
    fmt::print("AST\n");
    for(AstTypes child : children){
        AstType type = static_cast<AstType>(child.index()+1);
        switch(type){
            case luna::AstType::VAR_DECL: {
                if(std::get<std::shared_ptr<VarDecl>>(child).get()->get_name() == "__end_of_ast__"){
                    return;
                }
                fmt::print("|- Var Decl: {}\n", std::get<std::shared_ptr<VarDecl>>(child).get()->get_name());
            } break;
            case luna::AstType::VAR_ASSIGN: {
                fmt::print("|- Var Assign: {} = {}\n", std::get<std::shared_ptr<VarAssign>>(child).get()->get_name(), std::get<std::shared_ptr<VarAssign>>(child).get()->get_value());
            } break;
            case luna::AstType::EXPR: {} break;
            case luna::AstType::STMT: {} break;
            case luna::AstType::FUNC_DECL: {} break;
            case luna::AstType::ROOT:
            default: {
                fmt::print("UNREACHABLE AST TYPE: {}\n", (int)type);
            } break;
        }
    }
}
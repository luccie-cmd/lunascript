#include "sema.hh"

void luna::SeMa::check(bool is_body){
    if(_ast.get_type() != AstType::ROOT){
        _diag.ICE("Couldn't verify the AST\nreason: No root defined!\n");
    }
    std::vector<AstTypes> children = _ast.get_children();
    for(AstTypes child : children){
        try {
            if (auto expr = std::get_if<Expr>(&child)) {
                check_expr(*expr);
            } else if (auto funcDecl = std::get_if<FuncDecl>(&child)) {
                if(is_body){
                    _diag.Error("Cannot declare functions inside of functions!\n");
                }
                check_func_decl(*funcDecl);
            } else if (auto varAssign = std::get_if<VarAssign>(&child)) {
                check_var_assign(*varAssign);
            } else if (auto varDecl = std::get_if<VarDecl>(&child)) {
                check_var_decl(*varDecl);
            } else {
                fmt::print("Unexpected variant type!\n");
            }
        } catch (const std::exception& ex) {
            _diag.Error("Exception occured: {}\n", ex.what());
        }
    }
    std::vector<luna::Token> args = {};
    declared_functions.push_back(std::make_pair("fmt_print", std::make_pair(TypeHint::I64, 1)));
    for(std::pair<std::string, std::vector<Token>> called_func : called_functions){
        usz call_size = called_func.second.size();
        std::string call_name = called_func.first;
        // TODO: Check for type hinting if returning (which we aren't right now)
        std::pair<std::string, std::pair<luna::TypeHint, usz>> func = std::make_pair(call_name, std::make_pair(TypeHint::I64, call_size));
        if(std::find(declared_functions.begin(), declared_functions.end(), func) == declared_functions.end()){
            _diag.Error("No function named `{}` was found\n", call_name);
        }
    }
    for(Ast a : bodies){
        SeMa sema(a, _diag);
        sema.add_declared_functions(declared_functions);
        sema.check();
    }
}

void luna::SeMa::check_expr(Expr expr){
    switch(expr.get_type()){
        case ExprType::NONE: {
            _diag.ICE("ExprType Should never be None!!!\n");
        } break;
        case ExprType::CALL: {
            // We handle this after we've done all the function decleration parsing
            called_functions.push_back(std::make_pair(expr.call_get_name(), expr.call_get_operands()));
        } break;
        default: {
            _diag.ICE("Unhaneled or unknown ExprType with int type: {}\n", static_cast<int>(expr.get_type()));
        } break;
    }
}

void luna::SeMa::check_func_decl(FuncDecl decl){
    std::pair<std::string, std::pair<luna::TypeHint, usz>> func = std::make_pair(decl.get_name(), std::make_pair(decl.get_typehint(), decl.get_arity()));
    if(std::find(declared_functions.begin(), declared_functions.end(), func) != declared_functions.end()){
        _diag.Error("Cannot redefine an already existing function `{}`!\n", decl.get_name());
    }
    declared_functions.push_back(func);
    Ast body = decl.get_body();
    bodies.push_back(body);
}

void luna::SeMa::check_var_decl(VarDecl decl){
    std::string name = decl.get_name();
    if(std::find(varDecl_names.begin(), varDecl_names.end(), name) != varDecl_names.end()){
        _diag.Error("Cannot redefine an already existing variable `{}`!\n", name);
    }
    varDecl_names.push_back(name);
}

void luna::SeMa::check_var_assign(VarAssign assign){
    std::string name = assign.get_name();
    if(std::find(varDecl_names.begin(), varDecl_names.end(), name) == varDecl_names.end()){
        _diag.Error("Cannot assign to non existant variable `{}`!\n", name);
    }
}
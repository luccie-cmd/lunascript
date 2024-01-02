#include "sema.hh"

std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> find_var_decl_by_name(luna::Context _ctx, std::vector<luna::AstTypes> ast, std::string name){
    for(luna::AstTypes child : ast){
        if(child.index()+1 != (int)luna::AstType::VAR_DECL && child.index()+1 != (int)luna::AstType::VAR_DECLASSIGN)
            continue;
        
        if(child.index()+1 == (int)luna::AstType::VAR_DECL && std::get<std::shared_ptr<luna::VarDecl>>(child).get()->get_name() == name)
            return std::get<std::shared_ptr<luna::VarDecl>>(child);
        else if(child.index()+1 == (int)luna::AstType::VAR_DECLASSIGN && std::get<std::shared_ptr<luna::VarDeclAssign>>(child).get()->get_name() == name)
            return std::get<std::shared_ptr<luna::VarDeclAssign>>(child);
        
    }
    _ctx.diag.Error("Found no variable declerations with name {}\n", name);
    return std::make_shared<luna::VarDecl>(name);
}

void luna::Sema::analyse_astTypes(AstTypes child){
    AstType type = static_cast<AstType>(child.index()+1);
    switch(type){
        case luna::AstType::VAR_DECL: {
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDecl>>(child)->get_name()) != _sctx.declared_variables.end()){
                _ctx.diag.Error("Variable `{}` is already defined!\n", std::get<std::shared_ptr<VarDecl>>(child)->get_name());
            }
            _sctx.declared_variables.push_back(std::get<std::shared_ptr<VarDecl>>(child)->get_name());
        } break;
        case luna::AstType::VAR_ASSIGN: {
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarAssign>>(child)->get_name()) == _sctx.declared_variables.end()){
                _ctx.diag.Error("Cannot assign to undeclared variable `{}`!\n", std::get<std::shared_ptr<VarAssign>>(child)->get_name());
            }
            std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> decl = find_var_decl_by_name(_ctx, _sctx.current_func_body, std::get<std::shared_ptr<VarAssign>>(child)->get_name());
            if(decl.index() != 0){
                fmt::print("{}\n", decl.index());
                _ctx.diag.ICE("bad std::variant index returned!\n");
            }
            auto var_decl = std::get<std::shared_ptr<luna::VarDecl>>(decl);
            var_decl.get()->set_arg_type(std::get<std::shared_ptr<VarAssign>>(child)->get_value()._type == TokenType::NUMBER ? ArgType::I64 : ArgType::PTR);
        } break;
        case luna::AstType::VAR_DECLASSIGN: {
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name()) != _sctx.declared_variables.end()){
                _ctx.diag.Error("Variable `{}` is already defined!\n", std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            }
            _sctx.declared_variables.push_back(std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name()) == _sctx.declared_variables.end()){
                _ctx.diag.Error("Cannot assign to undeclared variable `{}`!\n", std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            }

            std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> decl = find_var_decl_by_name(_ctx, _sctx.current_func_body, std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            if(decl.index() != 1){
                fmt::print("{}\n", decl.index());
                _ctx.diag.ICE("bad std::variant index returned!\n");
            }
            auto var_decl = std::get<std::shared_ptr<luna::VarDeclAssign>>(decl);
            var_decl.get()->set_arg_type(std::get<std::shared_ptr<VarDeclAssign>>(child)->get_value()._type == TokenType::NUMBER ? ArgType::I64 : ArgType::PTR);
        } break;
        case luna::AstType::EXPR: {
            auto expr = std::get<ExprTypes>(child);
            ExprType exprType = static_cast<ExprType>(expr.index());
            switch (exprType) {
                case ExprType::CALL: {
                    CallExpr *call = std::get<std::shared_ptr<CallExpr>>(expr).get();
                    std::string call_name = call->get_name();
                    std::vector<Token> call_arguments = call->get_operands();
                    usz call_arity = call_arguments.size();
                    bool found = false;
                    std::pair<std::string, std::vector<std::pair<std::string, std::string>>> found_function;
                    for(std::pair<std::string, std::vector<std::pair<std::string, std::string>>> func : _sctx.declared_functions){
                        std::string func_name = func.first;
                        if(func_name == call_name && call_arity == func.second.size()){
                            found = true;
                            found_function = func;
                            break;
                        }
                    }
                    if(!found){
                        _ctx.diag.Error("Cannot call to non existant function `{}` with argument number `{}`\n", call_name, call_arity);
                    }
                    for(usz i = 0; i < call_arity; ++i){
                        Token call_arg = call_arguments.at(i);
                        std::string func_arg = found_function.second.at(i).first;
                        if(func_arg == "str"){
                            if(call_arg._type != TokenType::STRING){
                                _ctx.diag.Error("{}: Expected a string but got `{}`\n", call_arg.loc.to_str(), call_arg._value);
                            }
                        } else if(func_arg == "int"){
                            if(call_arg._type != TokenType::NUMBER){
                                _ctx.diag.Error("{}: Expected a number but got `{}`\n", call_arg.loc.to_str(), call_arg._value);
                            }
                        } else if(func_arg == "id"){
                            if(call_arg._type != TokenType::ID){
                                _ctx.diag.Error("{}: Expected an identifier but got `{}`\n", call_arg.loc.to_str(), call_arg._value);
                            }
                        } else{
                            _ctx.diag.ICE("Unimplemented function argument type `{}`\n", func_arg);
                        }
                    }
                } break;
                default: {
                    _ctx.diag.ICE("UNREACHABLE EXPRESSION TYPE: {}\n", (int)exprType);
                } break;
            }
        } break;
        case luna::AstType::STMT: {
            auto stmt = std::get<StmtTypes>(child);
            StmtType stmtType = static_cast<StmtType>(stmt.index());
            switch (stmtType) {
                case StmtType::RETURN: {
                    if(!_sctx.is_body){
                        _ctx.diag.Error("Cannot return in non function body!\n");
                    }
                    std::shared_ptr<ReturnStmt> ret_stmt = std::get<std::shared_ptr<ReturnStmt>>(stmt);
                    if(ret_stmt.get()->get_return_value() != "None"){
                        if(!string_is_int(ret_stmt.get()->get_return_value())){
                            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), ret_stmt.get()->get_return_value()) == _sctx.declared_variables.end()){
                                _ctx.diag.Error("Cannot return undeclared variable `{}`\n", ret_stmt.get()->get_return_value());
                            }
                        }
                    }
                } break;
                case StmtType::BLOCK:
                default: {
                    _ctx.diag.ICE("UNREACHABLE STATEMENT TYPE: {}\n", (int)stmtType);
                } break;
            }
        } break;
        case luna::AstType::FUNC_DECL: {
            if(_sctx.is_body){
                _ctx.diag.Error("Cannot define function `{}` inside of function `{}`!\n", std::get<std::shared_ptr<FuncDecl>>(child)->get_name(), _sctx.defined_function);
            }
            std::pair<std::string, std::vector<std::pair<std::string, std::string>>> func_decl = {std::get<std::shared_ptr<FuncDecl>>(child)->get_name(), std::get<std::shared_ptr<FuncDecl>>(child)->get_func_arguments()};
            if(std::find(_sctx.declared_functions.begin(), _sctx.declared_functions.end(), func_decl) != _sctx.declared_functions.end()){
                _ctx.diag.Error("Function `{}` is already defined!\n", std::get<std::shared_ptr<FuncDecl>>(child)->get_name());
            }
            if(std::get<std::shared_ptr<FuncDecl>>(child)->get_name() == "main"){
                std::get<std::shared_ptr<FuncDecl>>(child)->set_linkage(Linkage::EXPORTED);
            }
            _sctx.declared_functions.push_back(func_decl);

            auto& funcDecl = std::get<std::shared_ptr<FuncDecl>>(child);
            for(std::pair<std::string, std::string> arg : funcDecl->get_func_arguments()){
                if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), arg.second) != _sctx.declared_variables.end()){
                    _ctx.diag.Error("Function argument `{}` is already defined or there has been a variable defined with the same name!\n", arg.second);
                }
                _sctx.declared_variables.push_back(arg.second);
            }

            SemaContext sctx_old = _sctx;
            analyse_blockStmt(funcDecl->get_name(), funcDecl->get_body());
            _sctx = sctx_old;
        } break;
        case luna::AstType::ROOT:
        default: {
            _ctx.diag.ICE("UNREACHABLE AST TYPE: {}\n", (int)type);
        } break;
    }
}

void luna::Sema::analyse(){
    _sctx.build = false;
    _sctx.is_body = false;
    for (AstTypes child : _ast.get_children()) {
        if (auto assign = std::get_if<std::shared_ptr<VarAssign>>(&child)) {
            if (assign->get()->get_name() == "build") {
                _ctx.diag.ICE("BUILD SYSTEM IS TEMPORARALY DISABLED AND NOT YET IMPLEMENTED\n");
                _sctx.build = true;
            }
        }
    }
    if(_sctx.build){
        setup_sema_build();
    }
    _sctx.current_func_body = _ast.get_children();
    for(AstTypes child : _ast.get_children()){
        analyse_astTypes(child);
    }
}
void luna::Sema::analyse_blockStmt(std::string name, BlockStmt stmt){
    _sctx.is_body = true;
    _sctx.defined_function = name;
    _sctx.current_func_body = stmt.get_body();
    for(AstTypes child : stmt.get_body()){
        analyse_astTypes(child);
    }
    _sctx.is_body = false;
}

void luna::Sema::setup_sema_build(){
    _sctx.declared_variables.push_back("build");
    _sctx.declared_variables.push_back("project_name");
    _sctx.declared_variables.push_back("project_version");
    _sctx.declared_variables.push_back("destination_dir");
    _sctx.declared_variables.push_back("libs_path");

    // _sctx.declared_functions.push_back("build");
    // _sctx.declared_functions.push_back("set");
    // _sctx.declared_functions.push_back("message");
    // _sctx.declared_functions.push_back("add_git_repo");
    // _sctx.declared_functions.push_back("add_library");
    // _sctx.declared_functions.push_back("include_directory");

}
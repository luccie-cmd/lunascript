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

luna::SemaContext luna::Sema::analyse_astTypes(AstTypes child, SemaContext sctx){
    SemaContext _sctx = sctx;
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
            std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> decl;
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarAssign>>(child)->get_name()) != _sctx.declared_variables.end()){
                decl = std::make_shared<VarDecl>(std::get<std::shared_ptr<VarAssign>>(child)->get_name());
            } else{
                decl = find_var_decl_by_name(_ctx, _sctx.current_func_body, std::get<std::shared_ptr<VarAssign>>(child)->get_name());
            }
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

            std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> decl;

            if (std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name()) != _sctx.declared_variables.end()) {
                decl = std::make_shared<VarDeclAssign>(std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_value());
            } else {
                decl = find_var_decl_by_name(_ctx, _sctx.current_func_body, std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            }
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
                        _ctx.diag.Info("Function that called the return `{}`\n", _sctx.defined_function);
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

            FuncDecl funcDecl = *std::get<std::shared_ptr<FuncDecl>>(child).get();

            SemaContext new_sctx;
            new_sctx.declared_variables = _sctx.declared_variables;
            new_sctx.declared_functions = _sctx.declared_functions;
            new_sctx.is_body = true;
            new_sctx.defined_function = funcDecl.get_name();
            // Copy over the function arguments as variable declerations
            for(auto arg : funcDecl.get_func_arguments()){
                if(std::find(new_sctx.declared_variables.begin(), new_sctx.declared_variables.end(), arg.second) != new_sctx.declared_variables.end()){
                    _ctx.diag.Error("Variable `{}` is already defined or there is a variable with the same name!\n", arg.second);
                }
                VarDecl varDecl = *std::make_shared<VarDecl>(arg.second).get();
                varDecl.set_arg_type(arg.first == "int" ? ArgType::I64 : ArgType::PTR);
                auto sharedvarDecl = std::make_shared<VarDecl>(varDecl);
                new_sctx.current_func_body.push_back(sharedvarDecl);
            }
            
            if(funcDecl.get_linkage() != Linkage::IMPORTED){
                BlockStmt body = funcDecl.get_body();
                analyse_blockStmt(new_sctx, body);
            }
        } break;

        case luna::AstType::ROOT:
        default: {
            _ctx.diag.ICE("UNREACHABLE AST TYPE: {}\n", (int)type);
        } break;
    }
    return _sctx;
}

void luna::Sema::analyse(){
    SemaContext _sctx;
    _sctx.is_body = false;
    _sctx.current_func_body = _ast.get_children();
    for(AstTypes child : _ast.get_children()){
        _sctx = analyse_astTypes(child, _sctx);
    }
}

void luna::Sema::analyse_blockStmt(SemaContext sctx, BlockStmt blkStmt){
    SemaContext copy_sctx = sctx;
    
    // Copy the body of the BlockStmt to the new context
    std::vector<AstTypes> copied_body = copy_vector(sctx.current_func_body, blkStmt.get_body());

    // Analyze the copied body
    for(const AstTypes& child : copied_body){
        fmt::print("Index {} (BlockStmt)\n", child.index()+1);
        copy_sctx = analyse_astTypes(child, copy_sctx);
    }
}
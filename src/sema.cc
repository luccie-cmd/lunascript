#include "sema.hh"

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
        } break;
        case luna::AstType::VAR_DECLASSIGN: {
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name()) != _sctx.declared_variables.end()){
                _ctx.diag.Error("Variable `{}` is already defined!\n", std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            }
            _sctx.declared_variables.push_back(std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            if(std::find(_sctx.declared_variables.begin(), _sctx.declared_variables.end(), std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name()) == _sctx.declared_variables.end()){
                _ctx.diag.Error("Cannot assign to undeclared variable `{}`!\n", std::get<std::shared_ptr<VarDeclAssign>>(child)->get_name());
            }
        } break;
        case luna::AstType::EXPR: {
            auto expr = std::get<ExprTypes>(child);
            ExprType exprType = static_cast<ExprType>(expr.index());
            switch (exprType) {
                case ExprType::CALL: {
                    CallExpr *call = std::get<std::shared_ptr<CallExpr>>(expr).get();
                    // TODO: Find a way to check for arity
                    if(std::find(_sctx.declared_functions.begin(), _sctx.declared_functions.end(), call->get_name()) == _sctx.declared_functions.end()){
                        _ctx.diag.Error("Cannot call to undeclared function `{}`\n", call->get_name());
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
                case StmtType::IMPORT: {
                    std::vector<std::string> imports = {
                        "std",
                    };
                    if(std::find(imports.begin(), imports.end(), std::get<ImportStmt*>(stmt)->get_name()) == imports.end()){
                        _ctx.diag.Note("Avaliable import types are: {}\n", imports.at(0));
                        _ctx.diag.Error("Invalid import type: {}\n", std::get<ImportStmt*>(stmt)->get_name());
                    }
                    if(std::get<ImportStmt*>(stmt)->get_name() == "std"){
                        _sctx.declared_functions.push_back("print");
                        std::get<ImportStmt*>(stmt)->functions.push_back("print");
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
            if(std::find(_sctx.declared_functions.begin(), _sctx.declared_functions.end(), std::get<std::shared_ptr<FuncDecl>>(child)->get_name()) != _sctx.declared_functions.end()){
                _ctx.diag.Error("Function `{}` is already defined!\n", std::get<std::shared_ptr<FuncDecl>>(child)->get_name());
            }
            _sctx.declared_functions.push_back(std::get<std::shared_ptr<FuncDecl>>(child)->get_name());

            SemaContext sctx_old = _sctx;
            analyse_blockStmt(std::get<std::shared_ptr<FuncDecl>>(child)->get_name(), std::get<std::shared_ptr<FuncDecl>>(child)->get_body());
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
                _sctx.build = true;
            }
        }
    }
    if(_sctx.build){
        setup_sema_build();
    }
    for(AstTypes child : _ast.get_children()){
        analyse_astTypes(child);
    }
}
void luna::Sema::analyse_blockStmt(std::string name, BlockStmt stmt){
    _sctx.is_body = true;
    _sctx.defined_function = name;
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

    _sctx.declared_functions.push_back("build");
    _sctx.declared_functions.push_back("set");
    _sctx.declared_functions.push_back("message");
    _sctx.declared_functions.push_back("add_git_repo");
    _sctx.declared_functions.push_back("add_library");
    _sctx.declared_functions.push_back("include_directory");
}
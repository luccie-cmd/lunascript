#include "parser.hh"

#define FUNC_DECL_NAME "func"
#define VAR_DECL_NAME "var"

luna::BlockStmt luna::Parser::parse_block(){
    Token current = _lexer.next_token();
    if(current._type != TokenType::OPEN_CURLY){
        _ctx.diag.Error("{}: Expected '{}' to begin block\n", current.loc.to_str(), '{');
    }
    current = _lexer.peek();
    BlockStmt stmt(ast.get_scope());
    // Check for exit condition
    if(current._type == TokenType::CLOSE_CURLY){
        _lexer.next_token();
        return stmt;
    }
    while(current._type != TokenType::CLOSE_CURLY && !last_ast_node){
        AstTypes types = next_node();
        stmt.add_body(types);
        stmt.populate_curent_scope();
        current = _lexer.peek();
    }
    current = _lexer.next_token();
    if(current._type != TokenType::CLOSE_CURLY){
        _ctx.diag.ICE("Lexer::peek() or Lexer::next_token() stopped working!\n");
    }
    return stmt;
}

std::vector<std::string> allowed_func_arguments_types = {
    "str",
    "id",
    "int",
};

std::shared_ptr<luna::FuncDecl> luna::Parser::parse_func_decl(Linkage linkage){
    if(current._value != FUNC_DECL_NAME){
        _ctx.diag.Error("{}: Expected name `func` but got `{}`\n", current.loc.to_str(), current._value);
    }
    current = _lexer.next_token();
    std::string name = current._value;
    current = _lexer.next_token();
    if(current._type != TokenType::OPEN_PAREN){
        _ctx.diag.Error("{} Exptected open parentheses for function `{}` here\n", current.loc.to_str(), name);
    }
    current = _lexer.next_token();
    std::vector<std::string> arguments;
    while(current._type != TokenType::CLOSE_PAREN){
        if(current._type != TokenType::ID){
            _ctx.diag.Error("Expected `id` before `{}`\n", current._value);
        }
        if(std::find(allowed_func_arguments_types.begin(), allowed_func_arguments_types.end(), current._value) == allowed_func_arguments_types.end()){
            _ctx.diag.Note("Allowed types are:\n");
            for(std::string afat : allowed_func_arguments_types)
                _ctx.diag.Info("    {}\n", afat);
            _ctx.diag.Error("Expected a valid type but got `{}`\n", current._value);
        }
        arguments.push_back(current._value);
        current = _lexer.next_token();
        if(current._type == TokenType::CLOSE_PAREN){
            break;
        }
        if(current._type != TokenType::COMMA){
            _ctx.diag.Error("Expected `,` before `{}`\n", current._value);
        }
        current = _lexer.next_token();
    }
    if(current._type != TokenType::CLOSE_PAREN){
        _ctx.diag.Error("{}: Expected ')' to close function `{}` arguments\n", current.loc.to_str(), name);
    }
    // If we are tyring to import something don't handle function body
    BlockStmt block(ast.get_scope());
    if(linkage != Linkage::IMPORTED){
        block = parse_block();
    }
    block.populate_curent_scope();
    return std::make_shared<FuncDecl>(name, block, linkage, arguments);
}

std::variant<std::shared_ptr<luna::VarDecl>, std::shared_ptr<luna::VarDeclAssign>> luna::Parser::parse_var_decl(){
    std::string name = _lexer.next_token()._value;

    current = _lexer.next_token();
    if(current._type != TokenType::SEMICOLON && current._type != TokenType::EQUAL){
        current.loc._col -= current._value.size();
        _ctx.diag.Error("{}: Expected initializer before '{}'\n", current.loc.to_str(), current._value);
    }
    if(current._type == TokenType::EQUAL){
        // Also an assignment
        current = _lexer.next_token();
        Token value = current;
        current = _lexer.next_token();
        if(current._type != TokenType::SEMICOLON){
            current.loc._col -= current._value.size();
            _ctx.diag.Error("{}: Expected ';' before '{}'\n", current.loc.to_str(), current._value);
        }
        return std::make_shared<VarDeclAssign>(name, value);
    }
    return std::make_shared<VarDecl>(name);
}

std::shared_ptr<luna::VarAssign> luna::Parser::parse_var_assign(){
    std::string name = current._value;
    current = _lexer.next_token();
    if(current._type != TokenType::EQUAL){
        _ctx.diag.ICE("Lexer Peek stopped working\n");
    }
    current = _lexer.next_token();
    Token value = current;
    current = _lexer.next_token();
    if(current._type != TokenType::SEMICOLON){
        current.loc._col -= current._value.size();
        _ctx.diag.Error("{}: Expected ';' before '{}'\n", current.loc.to_str(), current._value);
    }
    return std::make_shared<VarAssign>(name, value);
}

std::shared_ptr<luna::CallExpr> luna::Parser::parse_call_expr(){
    std::string name = current._value;
    current = _lexer.next_token();
    if(current._type != TokenType::OPEN_PAREN){
        _ctx.diag.ICE("Lexer Peek stopped working\n");
    }
    std::vector<Token> arguments;
    current = _lexer.next_token();
    while(current._type != TokenType::CLOSE_PAREN){
        arguments.push_back(current);
        current = _lexer.next_token();
        if(current._type == TokenType::CLOSE_PAREN){
            current = _lexer.next_token();
            break;
        }
        if(current._type != TokenType::COMMA){
            _ctx.diag.Error("{}: Expected `,` before `{}`\n", current.loc.to_str(), current._value);
        }
        current = _lexer.next_token();
    }
    if(current._type == TokenType::CLOSE_PAREN){
        current = _lexer.next_token();
    }
    if(current._type != TokenType::SEMICOLON){
        _ctx.diag.Error("{}: Expected `;` before `{}`\n", current.loc.to_str(), current._value);
    }
    
    return std::make_shared<CallExpr>(name, arguments);
}

luna::AstTypes luna::Parser::next_node(){
    current = _lexer.next_token();
    if(current._type == TokenType::TT_EOF){
        last_ast_node = true;
        std::shared_ptr<VarDecl> decl = std::make_shared<VarDecl>("__end__");
        return decl;

    }
    // First off handle variable declerations
    if(current._value == VAR_DECL_NAME){
        std::variant<std::shared_ptr<VarDecl>, std::shared_ptr<VarDeclAssign>> decl = parse_var_decl();
        switch(decl.index()+1){
            case AstType::VAR_DECL: return std::get<std::shared_ptr<VarDecl>>(decl);
            case AstType::VAR_DECLASSIGN: return std::get<std::shared_ptr<VarDeclAssign>>(decl);
        }
    } else if(_lexer.peek()._type == TokenType::EQUAL && current._type == TokenType::ID){
        return parse_var_assign();
    } else if(current._value == FUNC_DECL_NAME){
        return parse_func_decl(Linkage::INTERNAL);
    } else if(current._value == "import"){
        current = _lexer.next_token();
        std::shared_ptr<FuncDecl> func_decl = parse_func_decl(Linkage::IMPORTED);
        current = _lexer.next_token();
        if(current._type != TokenType::SEMICOLON){
            _ctx.diag.Error("{}: Expected `;` before `{}`\n", current.loc.to_str(), current._value);
        }
        return func_decl;
    } else if(current._value == "export"){
        current = _lexer.next_token();
        return parse_func_decl(Linkage::EXPORTED);
    } else if(current._value == "return"){
        current = _lexer.next_token();
        if(current._type == TokenType::SEMICOLON){
            return std::make_shared<ReturnStmt>("None");
        }
        std::string ret_value = current._value;
        current = _lexer.next_token();
        if(current._type != TokenType::SEMICOLON){
            _ctx.diag.Error("{}: Expected `;` before `{}`\n", current.loc.to_str(), current._value);
        }
        return std::make_shared<ReturnStmt>(ret_value);
    } else if(current._type == TokenType::ID && _lexer.peek()._type == TokenType::OPEN_PAREN){
        return parse_call_expr();
    } else{
        current.loc._col -= current._value.size();
        _ctx.diag.Error("{}: Unexpected token `{}`, {}\n", current.loc.to_str(), current._value, (int)current._type);
    }

    ast.print();
    _ctx.diag.ICE("Unreturned Ast node!\n");
    std::exit(0);
}

void luna::Parser::parse(){
    while(!last_ast_node){
        ast.add_child(next_node());
        ast.populate_scope();
    }
}

luna::Ast luna::Parser::get_ast(){
    ast.populate_scope();
    return ast;
}
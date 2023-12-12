#include "parser.hh"

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
    while(current._type != TokenType::CLOSE_CURLY){
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

luna::AstTypes luna::Parser::next_node(){
    Token current = _lexer.next_token();
    if(current._type == TokenType::TT_EOF){
        last_ast_node = true;
        std::shared_ptr<VarDecl> decl = std::make_shared<VarDecl>("__end__");
        return decl;

    }
    // First off handle variable declerations
    if(current._value == "var"){
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
            auto declassign = std::make_shared<VarDeclAssign>(name, value);
            return declassign;
        } else{
            // Just a decl
            return std::make_shared<VarDecl>(name);
        }
    } else if(_lexer.peek()._type == TokenType::EQUAL && current._type == TokenType::ID){
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
        auto assign = std::make_shared<VarAssign>(name, value);
        return assign;
    } else if(current._value == "func"){
        current = _lexer.next_token();
        std::string name = current._value;
        current = _lexer.next_token();
        if(current._type != TokenType::OPEN_PAREN){
            _ctx.diag.Error("{} Exptected open parentheses for function `{}` here\n", current.loc.to_str(), name);
        }
        current = _lexer.next_token();
        while(current._type != TokenType::CLOSE_PAREN){
            _ctx.diag.ICE("{}: TODO: Handle function decl arguments!\n", current.loc.to_str());
        }
        if(current._type != TokenType::CLOSE_PAREN){
            _ctx.diag.Error("{}: Expected ')' to close function `{}` arguments\n", current.loc.to_str(), name);
        }
        BlockStmt block = parse_block();
        block.populate_curent_scope();
        auto decl = std::make_shared<FuncDecl>(name, block);
        return decl;
    } else if(current._type == TokenType::ID && _lexer.peek()._type == TokenType::OPEN_PAREN){
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

        // last_ast_node = true;
        std::shared_ptr<CallExpr> expr = std::make_shared<CallExpr>(name, arguments);
        return expr;
    } else if(current._value == "import"){
        std::string name = _lexer.next_token()._value;
        ImportStmt* stmt = new ImportStmt(name);
        return stmt;
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
#include "parser.hh"

luna::AstTypes luna::Parser::next_node(){
    auto end_decl = std::make_shared<VarDecl>("__end_of_ast__");
    Token current = _lexer.next_token();
    if(current._type == TokenType::TT_EOF){
        last_ast_node = true;
        return end_decl;
    }
    // First off handle variable declerations
    if(current._value == "var"){
        std::string name = _lexer.next_token()._value;
        auto decl = std::make_shared<VarDecl>(name);
        ast.add_child(decl);

        current = _lexer.next_token();
        if(current._type != TokenType::SEMICOLON && current._type != TokenType::EQUAL){
            current.loc._col -= current._value.size();
            _ctx.diag.Error("{}: Expected initializer before '{}'\n", current.loc.to_str(), current._value);
        }
        if(current._type == TokenType::EQUAL){
            current = _lexer.next_token();
            std::string value = current._value;
            current = _lexer.next_token();
            if(current._type != TokenType::SEMICOLON){
                current.loc._col -= current._value.size();
                _ctx.diag.Error("{}: Expected ';' before '{}'\n", current.loc.to_str(), current._value);
            }
            auto assign = std::make_shared<VarAssign>(name, value);
            ast.add_child(assign);
        }
        return next_node();
    } else if(_lexer.peek()._type == TokenType::EQUAL && current._type == TokenType::ID){
        std::string name = current._value;
        current = _lexer.next_token();
        if(current._type != TokenType::EQUAL){
            _ctx.diag.ICE("Lexer Peek stopped working\n");
        }
        current = _lexer.next_token();
        std::string value = current._value;
        current = _lexer.next_token();
        if(current._type != TokenType::SEMICOLON){
            current.loc._col -= current._value.size();
            _ctx.diag.Error("{}: Expected ';' before '{}'\n", current.loc.to_str(), current._value);
        }
        auto assign = std::make_shared<VarAssign>(name, value);
        ast.add_child(assign);
        return next_node();
    } else{
        current.loc._col -= current._value.size();
        _ctx.diag.Error("{}: Unexpected token\n", current.loc.to_str());
    }
    last_ast_node = true;
    return end_decl;
}

void luna::Parser::parse(){
    while(!last_ast_node){
        ast.add_child(next_node());
    }
}

luna::Ast luna::Parser::get_ast(){
    return ast;
}
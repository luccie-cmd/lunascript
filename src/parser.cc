#include "parser.hh"
#include <unordered_map>

// This contains everything for top level declerations
luna::Ast luna::Parser::nodes(){
    luna::Ast ret(AstType::ROOT);
    luna::Token next = vector_pop_back<Token>(_tokens);
    while(1){
        Token pref = next;
        if(pref._value == "func"){
            // Found a function decl
            // Skip the 'func' token and get the name
            std::string name = vector_pop_back<Token>(_tokens)._value;
            next = vector_pop_back<Token>(_tokens);
            if(next._type != TokenType::OPEN_PAREN){
                _diag.Error("{}: IN TOP LEVEL DECL: Found invalid token after function name decleration! `{}`\n", next.loc.to_str(), next._value);
            }
            // TODO: Parse function decleration arguments
            next = vector_pop_back<Token>(_tokens);
            while(next._type != TokenType::CLOSE_PAREN){
                fmt::print("{}: ERROR: Can't parse function decleration arguments yet!\n", next.loc.to_str());
                std::exit(1);
            }
            // skip the ')' token
            // next = vector_pop_back<Token>(_tokens);
            
            // Handle type hinting
            std::unordered_map<std::string, TypeHint> TypeHintStr = {
                {"i8", TypeHint::I8},
                {"i16", TypeHint::I16},
                {"i32", TypeHint::I32},
                {"i64", TypeHint::I64}
            };
            // check if we need to type check
            next = vector_pop_back<Token>(_tokens);
            TypeHint type_hint = TypeHint::I64;
            if(next._type == TokenType::COLON){
                next = vector_pop_back<Token>(_tokens);
                std::string hint = next._value;
                auto i = TypeHintStr.find(hint);
                if(i == TypeHintStr.end()){
                    fmt::print("{}: ERROR: Invalid type hint with name {}\n", next.loc.to_str(), hint);
                    std::exit(1);
                }
                type_hint = i->second;
                next = vector_pop_back<Token>(_tokens);
            }

            if(next._type != TokenType::OPEN_CURLY){
                _diag.Error("{}: IN TOP LEVEL DECL: Expected a `{}` but got {}\n", next.loc.to_str(), '{', next._value);
            }

            Ast body = parse_body();

            FuncDecl node(name, type_hint);
            node.set_body(body);
            ret.add_child(node);

        } else if(next._type == TokenType::ID){
            Token pref2 = next;
            next = vector_pop_back<Token>(_tokens);
            if(pref2._value == "var") {
                pref = next;
                next = vector_pop_back<Token>(_tokens);
                if(next._type == TokenType::EQUAL){
                    // Assignment found
                    std::string lhs = pref._value;
                    next = vector_pop_back<Token>(_tokens);
                    // Check for comparison
                    if(next._type == TokenType::EQUAL){
                        _diag.Error("{}: IN TOP LEVEL DECL: Can't do comparison yet!\n", next.loc.to_str());
                    }
                    if(next._type != TokenType::STRING && next._type != TokenType::NUMBER){
                        _diag.Error("{}: IN TOP LEVEL DECL: Invalid assignment expression!\n", next.loc.to_str());
                    }
                    pref = next;
                    next = vector_pop_back<Token>(_tokens);
                    if(next._type != TokenType::SEMICOLON){
                        _diag.Error("{}: IN TOP LEVEL DECL: Expected semicolon at the end of an variable assignment!\n", pref.loc.to_str());
                    }

                    std::string name = lhs;
                    std::string value = pref._value;
                    VarDecl decl(name);
                    ret.add_child(decl);
                    VarAssign assign(name, value);
                    ret.add_child(assign);                    
                } else if(next._type == TokenType::SEMICOLON){
                    std::string& name = pref._value;
                    // No error reporting here since we do that above and below
                    VarDecl decl(name);
                    ret.add_child(decl);
                } else{
                    _diag.Error("{}: IN TOP LEVEL DECL: Expected `=` or `;` but got `{}`\n", next.loc.to_str(), next._value);
                }
            } else{
                _diag.Error("{}: IN TOP LEVEL DECL: Invalid token found `{}`\n", pref.loc.to_str(), pref._value);
            }
        } else if(next._type == TokenType::TT_EOF){
            break;
        } else{
            _diag.Error("{}: IN TOP LEVEL DECL: Invalid token found `{}`\n", pref.loc.to_str(), pref._value);
        }
        next = vector_pop_back<Token>(_tokens);
    }
    return ret;
}

std::vector<std::string> keyword_names = {
    "return",
};

// the INT type is an index in the statement kind
std::vector<std::pair<std::string, int>> keyword_pairs = {
    {"return", 1}, 
};

// This contains everything for body declerations
luna::Ast luna::Parser::parse_body(){
    luna::Ast ret(AstType::ROOT);
    luna::Token next = vector_pop_back<Token>(_tokens);
    Loc top_loc = next.loc;
    while(1){
        Token pref = next;
        if(next._type == TokenType::ID){
            bool keyword_found = false;
            for (const auto& pair : keyword_pairs) {
                if (pair.first == next._value) {
                    std::string name = next._value;
                    next = vector_pop_back<Token>(_tokens);
                    std::vector<Token> arguments;
                    while(next._type != TokenType::SEMICOLON){
                        arguments.push_back(next);
                        next = vector_pop_back<Token>(_tokens);
                    }
                    next = vector_pop_back<Token>(_tokens);
                    
                    keyword_found = true;
                    break;
                }
            }
            if(keyword_found){
                continue;
            }
            Token pref2 = next;
            next = vector_pop_back<Token>(_tokens);
            if(next._type == TokenType::OPEN_PAREN){
                // We found a function call (or an invalid token)
                // Skip the `(` token
                // Prepare stuff
                std::string name = pref._value;
                std::vector<Token> operands;

                // Loop for the operands
                while(next._type != TokenType::CLOSE_PAREN && next._type != TokenType::TT_EOF){
                    next = vector_pop_back<Token>(_tokens);
                    if(next._type == TokenType::CLOSE_PAREN){
                        break;
                    }
                    operands.push_back(next);
                    next = vector_pop_back<Token>(_tokens);
                    if(next._type != TokenType::COMMA && next._type != TokenType::CLOSE_PAREN){
                        _diag.Error("{}: ERROR IN BODY: Invalid token encountered! value: {}\n", next.loc.to_str(), next._value);
                    }
                }

                // Error handling for things
                // Save the location before overwriting it and forcoming that we jump N number of lines further
                Loc pref_loc = next.loc;
                pref_loc.update(' ');
                next = vector_pop_back<Token>(_tokens);
                if(next._type != TokenType::SEMICOLON){
                    _diag.Error("{}: ERROR IN BODY: Couldn't find a semicolon at the end of an expression!\n", pref_loc.to_str());
                }

                // Add this expression to the AST
                Expr expr(ExprType::CALL);
                for(Token t : operands) expr.call_add_operand(t);
                expr.call_set_name(name);
                ret.add_child(expr);
                // Variable decleration
            } else if(pref2._value == "var") {
                pref = next;
                next = vector_pop_back<Token>(_tokens);
                if(next._type == TokenType::EQUAL){
                    // Assignment found
                    std::string lhs = pref._value;
                    next = vector_pop_back<Token>(_tokens);
                    // Check for comparison
                    if(next._type == TokenType::EQUAL){
                        _diag.Error("{}: ERROR IN BODY: Can't do comparison yet!\n", next.loc.to_str());
                    }
                    if(next._type != TokenType::STRING && next._type != TokenType::NUMBER){
                        _diag.Error("{}: ERROR IN BODY: Invalid assignment expression!\n", next.loc.to_str());
                    }
                    pref = next;
                    next = vector_pop_back<Token>(_tokens);
                    if(next._type != TokenType::SEMICOLON){
                        _diag.Error("{}: ERROR IN BODY: Expected semicolon at the end of an variable assignment!\n", pref.loc.to_str());
                    }

                    std::string name = lhs;
                    std::string value = pref._value;
                    VarDecl decl(name);
                    ret.add_child(decl);
                    VarAssign assign(name, value);
                    ret.add_child(assign);                    
                } else if(next._type == TokenType::SEMICOLON){
                    std::string& name = pref._value;
                    // No error reporting here since we do that above and below
                    VarDecl decl(name);
                    ret.add_child(decl);
                } else{
                    _diag.Error("{}: ERROR IN BODY: Expected `=` or `;` but got `{}`\n", next.loc.to_str(), next._value);
                }
            } else if(next._type == TokenType::EQUAL){
                std::string name = pref._value;
                pref = next = vector_pop_back<Token>(_tokens);
                if(next._type != TokenType::STRING && next._type != TokenType::NUMBER){
                    _diag.Error("{}: ERROR IN BODY: Invalid assignment expression!\n", next.loc.to_str());
                }
                std::string value = next._value;
                next = vector_pop_back<Token>(_tokens);
                if(next._type != TokenType::SEMICOLON){
                    _diag.Error("{}: ERROR IN BODY: Expected semicolon at the end of an variable assignment!\n", pref.loc.to_str());
                }

                VarAssign assign(name, value);
                ret.add_child(assign);
            } else{
                _diag.Error("{}: ERROR IN BODY: Invalid token found `{}`\n", pref.loc.to_str(), pref._value);
            }
        } else if(next._type == TokenType::CLOSE_CURLY){
            break;
        } else if(next._type == TokenType::TT_EOF){
            _diag.Note("Declared here: {}\n", top_loc.to_str());
            _diag.Error("couldn't find closing curly brace!\n");
        } else{
            _diag.Error("{}: ERROR IN BODY: Invalid token found `{}`\n", pref.loc.to_str(), pref._value);
        }
        next = vector_pop_back<Token>(_tokens);
    }
    return ret;
}
#include "parser.hh"

luna::Ast luna::Parser::nodes(){
    luna::Ast ret(AstType::ROOT);
    // luna::Token current = vector_pop_back<Token>(_tokens);
    luna::Token next = vector_pop_back<Token>(_tokens);
    while(1){
        Token pref = next;
        if(pref._value == "func"){
            // Found a function decl
            // Skip the 'func' token and get the name
            std::string name = vector_pop_back<Token>(_tokens)._value;
            next = vector_pop_back<Token>(_tokens);
            if(next._type != TokenType::OPEN_PAREN){
                fmt::print("{}: ERROR: Found invalid token after function name decleration! `{}`\n", next.loc.to_str(), next._value);
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
                fmt::print("{}: ERROR: Expected a `{}` but got {}\n", next.loc.to_str(), '{', next._value);
                std::exit(1);
            }

            // fmt::print("Pasing body\n");
            Ast body = nodes();
            
            FuncDecl node(name, type_hint);
            node.set_body(body);
            ret.add_child(&node);

        } else if(next._type == TokenType::ID){
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
                        fmt::print("{}: ERROR: Invalid token encountered! value: {}\n", next.loc.to_str(), next._value);
                        std::exit(1);
                    }
                }

                // Error handling for things
                // Save the location before overwriting it and forcoming that we jump N number of lines further
                Loc pref_loc = next.loc;
                pref_loc.update(' ');
                next = vector_pop_back<Token>(_tokens);
                if(next._type != TokenType::SEMICOLON){
                    fmt::print("{}: ERROR: Couldn't find a semicolon at the end of an expression!\n", pref_loc.to_str());
                    std::exit(1);
                }

                // Add this expression to the AST
                Expr expr(ExprType::CALL);
                for(Token t : operands) expr.call_add_operand(t);
                expr.call_set_name(name);
                ret.add_child(&expr);
            } else{
                pref = next;
                next = vector_pop_back<Token>(_tokens);
                if(next._type == TokenType::EQUAL){
                    // Assignment found
                    fmt::print("Assignment\n");
                }
                fmt::print("{}: ERROR: Invalid token found `{}`\n", pref.loc.to_str(), pref._value);
                fmt::print("{}: INFO: Next token {}\n", next.loc.to_str(), next._value);
                std::exit(1);
            }
        } else if(next._type == TokenType::TT_EOF || next._type == TokenType::CLOSE_CURLY){
            break;
        } else{
            fmt::print("{}: ERROR: Invalid next token! value: {}\n", next.loc.to_str(), next._value);
            std::exit(1);
        }
        next = vector_pop_back<Token>(_tokens);
    }
    return ret;
}
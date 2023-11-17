#include "parser.hh"

luna::Ast luna::Parser::nodes(){
    luna::Ast ret(AstType::ROOT);
    luna::Token current = vector_pop_back<Token>(_tokens);
    luna::Token next = vector_pop_back<Token>(_tokens);
    while(1){
        if(next._type == TokenType::OPEN_PAREN && current._type == TokenType::ID){
            // We found a function call (or an invalid token)
            // Skip the `(` token
            // Prepare stuff
            std::string name = current._value;
            std::vector<Token> operands;

            // Loop for the operands
            while(next._type != TokenType::CLOSE_PAREN && next._type != TokenType::TT_EOF){
                next = vector_pop_back<Token>(_tokens);
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
            fmt::print("{}: ERROR: Invalid next token! value: {}\n", next.loc.to_str(), next._value);
            std::exit(1);
        }
        next = vector_pop_back<Token>(_tokens);
    }
    std::vector<AstTypes> c = ret.get_children();
    AstTypes a = c.at(0);
    Expr *ep = std::get<Expr*>(a);
    Expr e = *ep;
    fmt::print("{}\n", e.call_get_name());
    return ret;
}
#if !defined(LUNA_PARSER_HH)
#define LUNA_PARSER_HH
#include "core.hh"
#include "lexer.hh"
#include "ast.hh"

namespace luna{
    class Parser{
        public:
            Parser(Lexer lexer){
                _tokens = lexer.lex();
                reverse_vector_in_place<Token>(_tokens);
            }
            Ast nodes();
        private:
            std::vector<Token> _tokens;
    };
}

#endif // LUNA_PARSER_HH

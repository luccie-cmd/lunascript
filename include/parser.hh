#if !defined(LUNA_PARSER_HH)
#define LUNA_PARSER_HH
#include "core.hh"
#include "lexer.hh"
#include "ast.hh"

namespace luna{
    class Parser{
        public:
            Parser(Lexer lexer, Diag diag) :_diag(diag){
                _tokens = lexer.lex();
                reverse_vector_in_place<Token>(_tokens);
            }
            Ast nodes();
            Ast parse_body();
        private:
            std::vector<Token> _tokens;
            Diag _diag;
    };
}

#endif // LUNA_PARSER_HH

#if !defined(LUNA_LEXER_HH)
#define LUNA_LEXER_HH
#include "core.hh"
#include "diag.hh"
#include "token.hh"

namespace luna{
    class Lexer{
        public:
            Lexer(std::string file, std::string contents, Diag diag) :_contents(contents), idx(0), _diag(diag){
                if(contents.size() == 0){
                    fmt::print("\nInvalid lexical input\n");
                    exit(1);
                }
                _c = contents.at(0);
                loc = luna::Loc(file);
            }
            std::vector<Token> lex();
            Token next_token();
        private:
            void advance();
            void skip_whitespace();
            bool isskip(int c){ return (_c == ' ' || _c == '\n' || _c == '\r' || _c == '\t'); }
            char _c;
            usz idx;
            std::string _contents;
            Loc loc;
            Diag _diag;
    };
};

#endif // LUNA_LEXER_HH
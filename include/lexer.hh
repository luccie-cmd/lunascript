#if !defined(LUNA_LEXER_HH)
#define LUNA_LEXER_HH
#include "core.hh"
#include "diag.hh"
#include "token.hh"

namespace luna
{
    class Lexer
    {
    public:
        Lexer(Context ctx, std::string file, std::string contents) : _ctx(ctx), _contents(contents), idx(0)
        {
            if (contents.size() <= 0)
            {
                _ctx.diag.Error("Invalid lexical content provided!\n");
            }
            _c = contents.at(0);
            loc = luna::Loc(file);
        }
        // lex the whole file at once
        std::vector<Token> lex();
        // Peek one ahead
        Token peek();
        // Get the next token (or current idk)
        Token next_token();

    private:
        // Advance index by one no mater what
        void advance();
        // Skip any characters that we can skip
        void skip_whitespace();
        bool isskip(int c) { return (_c == ' ' || _c == '\n' || _c == '\r' || _c == '\t' || _c == '\v'); }
        // Current character
        char _c;
        // Index in content
        usz idx;
        // Content itself
        std::string _contents;
        // Current location
        Loc loc;
        // Add the current context
        Context _ctx;
    };
};

#endif // LUNA_LEXER_HH
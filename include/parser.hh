#if !defined(LUNA_PARSER_HH)
#define LUNA_PARSER_HH
#include "core.hh"
#include "lexer.hh"
#include "ast.hh"

namespace luna
{
    class Parser
    {
    private:
        Lexer _lexer;
        Context _ctx;
        bool last_ast_node;
        Ast ast;

    public:
        Parser(Context ctx, Lexer lexer) : _ctx(ctx), _lexer(lexer), last_ast_node(false) {}
        AstTypes next_node();
        Ast get_ast();
        BlockStmt parse_block();
        void parse();
    };
};

#endif // LUNA_PARSER_HH

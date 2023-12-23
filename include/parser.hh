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
        Token current;

    public:
        Parser(Context ctx, Lexer lexer) : _ctx(ctx), _lexer(lexer), last_ast_node(false), current(Token(TokenType::TT_EOF, std::string("\0"), Loc("None"))) {}
        AstTypes next_node();
        Ast get_ast();
        BlockStmt parse_block();
        std::shared_ptr<FuncDecl> parse_func_decl(Linkage linkage=Linkage::INTERNAL);
        std::variant<std::shared_ptr<VarDecl>, std::shared_ptr<VarDeclAssign>, std::shared_ptr<VarAssign>> parse_var_decl();
        void parse();
    };
};

#endif // LUNA_PARSER_HH

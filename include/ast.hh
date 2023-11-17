#if !defined(LUNA_AST_HH)
#define LUNA_AST_HH
#include <variant>
#include "core.hh"
#include "token.hh"

namespace luna{

    enum struct AstType {
        NONE,
        ROOT,
        EXPR,
    };

    enum struct ExprType {
        NONE,
        CALL,
    };

    class Expr;
    // Alias for the variant type
    using AstTypes = std::variant<Expr*>;

    class Ast {
    public:
        Ast(AstType type = AstType::NONE) : _type(type) {}
        void add_child(AstTypes child) { _children.push_back(child); }
        std::vector<AstTypes> get_children() { return _children; }

    private:
        AstType _type;
        std::vector<AstTypes> _children;
    };

    class Expr : public Ast {
    public:
        Expr(ExprType type = ExprType::NONE) : _type(type) {}
        // Call Expr
        void call_add_operand(const Token& tok) { operands.push_back(tok); }
        void call_set_name(std::string& name) { _name = name; }
        std::vector<Token> call_get_operands() { return operands; }
        std::string call_get_name() { return _name; };
    private:
        ExprType _type;
        // Call Expr
        std::string _name;
        std::vector<Token> operands;
    };
};

#endif // LUNA_AST_HH

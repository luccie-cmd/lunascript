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
        FUNC_DECL,
    };

    enum struct ExprType {
        NONE,
        CALL,
    };

    class Expr;
    class FuncDecl;
    // Alias for the variant type
    using AstTypes = std::variant<Expr*, FuncDecl*>;

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

    enum struct TypeHint{
        I8,
        I16,
        I32,
        I64,
    };
    class FuncDecl : public Ast {
    public:
        FuncDecl(std::string name, TypeHint hint) :_name(name), _hint(hint){}
        void set_body(Ast bod){ body = bod; }
        Ast get_body(){ return body; }
    private:
        std::string _name;
        TypeHint _hint;
        Ast body;
    };

};

#endif // LUNA_AST_HH

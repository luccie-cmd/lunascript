#if !defined(LUNA_AST_HH)
#define LUNA_AST_HH
#include <variant>
#include "core.hh"
#include "token.hh"

namespace luna{

    enum struct AstType : int {
        NONE,
        ROOT,
        EXPR,
        FUNC_DECL,
        VAR_ASSIGN,
        VAR_DECL,
    };

    enum struct ExprType : int {
        NONE,
        CALL,
    };

    class Expr;
    class FuncDecl;
    class VarAssign;
    class VarDecl;
    // Alias for the variant type
    using AstTypes = std::variant<Expr*, FuncDecl*, VarAssign*, VarDecl*>;

    class Ast {
    public:
        Ast(AstType type = AstType::NONE) : _type(type) {}
        void add_child(AstTypes child) { _children.push_back(child); }
        std::vector<AstTypes> get_children() { return _children; }
        void print();
        AstType get_type() { return _type; }
    private:
        AstType _type;
        std::vector<AstTypes> _children;
    };

    class Expr : public Ast {
    public:
        Expr(ExprType type = ExprType::NONE) :Ast(AstType::EXPR), _type(type) {}
        ExprType expr_get_type() { return _type; }
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
        FuncDecl(std::string name, TypeHint hint) :Ast(AstType::FUNC_DECL), _name(name), _hint(hint){}
        void set_body(Ast bod){ body = bod; }
        Ast get_body(){ return body; }
    private:
        std::string _name;
        TypeHint _hint;
        Ast body;
    };

    class VarAssign : public Ast{
    public:
        VarAssign(std::string& name, std::variant<std::string, int> value) :Ast(AstType::VAR_ASSIGN), _name(name), _value(value){}
    private:
        std::string _name;
        std::variant<std::string, int> _value;
    };
    class VarDecl : public Ast{
    public:
        VarDecl(std::string& name) :Ast(AstType::VAR_DECL), _name(name){}
    private:
        std::string _name;
    };
};

#endif // LUNA_AST_HH

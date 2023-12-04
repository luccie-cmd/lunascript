#if !defined(LUNA_AST_HH)
#define LUNA_AST_HH

#include "core.hh"
#include "token.hh"
#include <variant>

namespace luna {

    enum struct AstType{
        NONE,
        ROOT,
        EXPR,
        STATEMENT,
        FUNC_DECL,
        VAR_ASSIGN,
        VAR_DECL,
    };

    enum struct ExprType{
        NONE,
        CALL,
    };

    enum struct StmtType{
        NONE,
        RETURN,
    };

    class Ast;
    class Expr;
    class Stmt;
    class FuncDecl;
    class VarAssign;
    class VarDecl;

    // Forward declaration of AstTypes
    using AstTypes = std::variant<Expr, Stmt, FuncDecl, VarDecl, VarAssign>;

    class Ast {
    public:
        Ast(AstType type = AstType::NONE) : _type(type) {}
        void add_child(const AstTypes& child) { _children.push_back(child); }
        std::vector<AstTypes> get_children() const { return _children; }
        void print(std::string prefix="", std::string AstName="AST");
        AstType get_type() const { return _type; }
    private:
        AstType _type;
        std::vector<AstTypes> _children;
    };

    class Expr : public Ast {
    public:
        Expr(ExprType type = ExprType::NONE) : Ast(AstType::EXPR), _type(type) {}
        ExprType get_type() const { return _type; }
        // Call Expr
        void call_add_operand(const Token& tok) { operands.push_back(tok); }
        void call_set_name(const std::string& name) { _name = name; }
        std::vector<Token> call_get_operands() const { return operands; }
        std::string call_get_name() const { return _name; }
    private:
        ExprType _type;
        // Call Expr
        std::string _name;
        std::vector<Token> operands;
    };

    enum struct TypeHint {
        I8,
        I16,
        I32,
        I64,
        VOID,
    };
    
    enum struct FuncAttributes{
        NONE=0b00,
        NO_RETURN=0b01,
    };

    class FuncDecl : public Ast {
    public:
        FuncDecl(std::string name, TypeHint hint) : Ast(AstType::FUNC_DECL), _name(name), _hint(hint) {}
        void set_body(const Ast bod) { body = bod; }
        const Ast& get_body() const { return body; }
        const TypeHint& get_typehint() const { return _hint; }
        const std::string& get_name() const { return _name; }
        const std::string get_typehint_str() const { 
            switch (_hint){
                case TypeHint::I8: return "I8";
                case TypeHint::I16: return "I16";
                case TypeHint::I32: return "I32";
                case TypeHint::I64: return "I64";
            }
            return "INVALID";
        }
        bool returns() { return attributes != FuncAttributes::NO_RETURN; }
        usz get_arity(){ return 0; }
        void set_attribute(FuncAttributes att) { attributes = att; }
    private:
        std::string _name;
        TypeHint _hint;
        Ast body;
        FuncAttributes attributes;
    };

    class VarAssign : public Ast {
    public:
        VarAssign(std::string name, std::variant<std::string, int> value) : Ast(AstType::VAR_ASSIGN), _name(name), _value(value) {}
        const std::string& get_name() const { return _name; }
        template<typename T>
        const T& get_value() const { return std::get<T>(_value); }
    private:
        std::string _name;
        std::variant<std::string, int> _value;
    };

    class VarDecl : public Ast {
    public:
        VarDecl(std::string name) : Ast(AstType::VAR_DECL), _name(name) {}
        const std::string& get_name() const { return _name; }
    private:
        std::string _name;
    };

    class Stmt : public Ast{
        public: 
            Stmt(StmtType type=StmtType::NONE): Ast(AstType::STATEMENT), _type(type){}
            Stmt(std::vector<Token> args, StmtType type=StmtType::NONE) : Ast(AstType::STATEMENT), _type(type), arguments(args){}
            StmtType get_type() const { return _type; }
            std::vector<Token> get_arguments() const { return arguments; }
        private:
            std::vector<Token> arguments;
            StmtType _type;
    };

}

#endif // LUNA_AST_HH

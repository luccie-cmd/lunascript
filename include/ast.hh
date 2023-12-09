#if !defined(LUNA_AST_HH)
#define LUNA_AST_HH
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include "token.hh"

namespace luna
{
    enum AstType
    {
        ROOT,
        VAR_DECL,
        VAR_ASSIGN,
        EXPR,
        STMT,
        FUNC_DECL,
    };

    enum ExprType
    {
        CALL,
    };

    enum StmtType
    {
        BLOCK,
    };

    struct Shared_Ast
    {
        AstType _astType;
    };

    class VarDecl;
    class VarAssign;
    class CallExpr;
    class BlockStmt;
    class FuncDecl;

    using ExprTypes = std::variant<std::shared_ptr<CallExpr>>;
    using StmtTypes = std::variant<std::shared_ptr<BlockStmt>>;
    using AstTypes = std::variant<std::shared_ptr<VarDecl>, std::shared_ptr<VarAssign>, ExprTypes, StmtTypes, std::shared_ptr<FuncDecl>>;


    class Ast : public Shared_Ast
    {
    private:
        std::vector<AstTypes> children;

    public:
        Ast()
        {
            _astType = AstType::ROOT;
        }
        void add_child(AstTypes child) { children.push_back(child); }
        void print();
    };

    class VarDecl : public Shared_Ast
    {
    private:
        std::string _name;

    public:
        VarDecl(std::string name) : _name(name)
        {
            _astType = AstType::VAR_DECL;
        }
        const std::string& get_name() const { return _name; }
    };

    class VarAssign : public Shared_Ast
    {
    private:
        std::string _name;
        std::string _value;

    public:
        VarAssign(std::string name, std::string value) : _name(name), _value(value)
        {
            _astType = AstType::VAR_ASSIGN;
        }
        const std::string& get_name() const { return _name; }
        const std::string& get_value() const { return _value; }
    };

    struct Expr : public Shared_Ast
    {
        ExprType _exprType;
        void init()
        {
            _astType = AstType::EXPR;
        }
    };

    class CallExpr : public Expr
    {
    private:
        std::string _name;
        std::vector<Token> _operands;

    public:
        CallExpr(std::string name) : _name(name)
        {
            init();
            _exprType = ExprType::CALL;
        }
        void add_operand(Token token) { _operands.push_back(token); }
    };

    struct Stmt : public Shared_Ast
    {
        StmtType _stmtType;
        void init()
        {
            _astType = AstType::EXPR;
        }
    };

    class BlockStmt : public Stmt
    {
    private:
        std::vector<AstTypes> _body;

    public:
    };

    class FuncDecl : public Shared_Ast
    {
    private:
        std::string name;
        BlockStmt body;

    public:
    };
};

#endif // LUNA_AST_HH

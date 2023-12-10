#if !defined(LUNA_AST_HH)
#define LUNA_AST_HH
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include "token.hh"
#include <unordered_map>

namespace luna
{
    enum AstType
    {
        ROOT,
        VAR_DECL,
        VAR_DECLASSIGN,
        VAR_ASSIGN,
        FUNC_DECL,
        EXPR,
        STMT,
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
    class VarDeclAssign; // For decleration and assignments in 1 (really just an VarAssign but now Sema knows too also define it)
    class VarAssign;
    class CallExpr;
    class BlockStmt;
    class FuncDecl;

    using ExprTypes = std::variant<std::shared_ptr<CallExpr>>;
    using StmtTypes = std::variant<std::shared_ptr<BlockStmt>>;
    using AstTypes = std::variant<std::shared_ptr<VarDecl>, std::shared_ptr<VarDeclAssign>, std::shared_ptr<VarAssign>, std::shared_ptr<FuncDecl>, ExprTypes, StmtTypes>;

    struct Scope{
        std::unordered_map<std::string, std::string> variables;
        std::vector<std::string> declared_functions;
        void print();
    };

    class Ast : public Shared_Ast
    {
    private:
        Scope current;
        std::vector<AstTypes> children;

    public:
        Ast()
        {
            _astType = AstType::ROOT;
        }
        Scope& get_scope() { return current; }
        void add_child(AstTypes child) { children.push_back(child); }
        void populate_scope();
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
        CallExpr(std::string name, std::vector<Token> operands) :_name(name), _operands(operands){
            init();
            _exprType = ExprType::CALL;
        }
        void add_operand(Token token) { _operands.push_back(token); }
        const std::string get_name() const { return _name; }
        const std::vector<Token> get_operands() const { return _operands; }
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
        Scope current;
        Scope parent_scope;

    public:
        BlockStmt(Scope scope){
            parent_scope = scope;
            _stmtType = StmtType::BLOCK;
        }
        void add_body(AstTypes add) { _body.push_back(add); }
        void print();
        void populate_curent_scope();
        const std::vector<AstTypes>& get_body() const { return _body; }
    };

    class FuncDecl : public Shared_Ast
    {
    private:
        std::string _name;
        BlockStmt _body;

    public:
        FuncDecl(std::string name, BlockStmt body) :_name(name), _body(body){
            _astType = AstType::FUNC_DECL;
        }
        const std::string& get_name() const { return _name; }
        const BlockStmt get_body() const { return _body; }
    };

    class VarDeclAssign : public Shared_Ast{
    private:
        std::string _name;
        std::string _value;

    public:
        VarDeclAssign(std::string name, std::string value) : _name(name), _value(value)
        {
            _astType = AstType::VAR_DECLASSIGN;
        }
        const std::string& get_name() const { return _name; }
        const std::string& get_value() const { return _value; }
    };
};

#endif // LUNA_AST_HH

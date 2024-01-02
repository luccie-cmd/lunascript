#if !defined(LUNA_IRGEN_HH)
#define LUNA_IRGEN_HH
#include "core.hh"
#include "ast.hh"

namespace luna
{
    enum struct IrType
    {
        ALLOCA,
        STORE,
        LOAD,
        RET,
        PUSH,
        CALL,
    };
    class Ir;

    class IrInst
    {
    private:
        IrType _type;
        std::vector<std::pair<ArgType, std::string>> _arguments;

    public:
        IrInst(IrType type) : _type(type) {}
        void add_argument(std::pair<ArgType, std::string> arg) { _arguments.push_back(arg); }
    };
    class IrFunction
    {
    private:
        std::string name;
        ArgType ret_type;
        Linkage link;
        std::vector<IrInst> body;
        Ir *parrent_ir; // parrent_ir is used to allocate the strings
        std::vector<std::pair<std::string, std::string>> args;

    public:
        IrFunction(std::string _name, Ir *parrent, Linkage lnk, std::vector<std::pair<std::string, std::string>> func_args) : name(_name), parrent_ir(parrent), link(lnk), args(func_args) {}
        void generate(Context ctx, BlockStmt block_body, usz arguments);
    };
    class Ir
    {
    private:
        std::vector<std::string> string_list;
        std::vector<std::pair<ArgType, std::string>> var_list;
        std::vector<IrInst> top_level_insts;
        std::vector<IrFunction> functions;

    public:
        Ir() {}
        void add_inst(IrInst inst) { top_level_insts.push_back(inst); }
        void add_string(std::string string) { string_list.push_back(string); }
        void add_var(std::pair<ArgType, std::string> var) { var_list.push_back(var); }
        void add_func(IrFunction func) { functions.push_back(func); }
        std::vector<std::string> get_string_list() { return string_list; }
        std::vector<std::pair<ArgType, std::string>> get_var_list() { return var_list; }
        std::vector<IrInst> get_insts() { return top_level_insts; };
        std::vector<IrFunction> get_functions() { return functions; }
    };
    class IrGen
    {
    private:
        Ir ir;
        Ast _ast;
        Context _ctx;
        VarDecl find_var_decl_by_name(std::string name);

    public:
        IrGen(Context ctx, Ast ast) : _ast(ast), _ctx(ctx) {}
        Ir get_ir();
        void gen_var_decl(VarDecl decl);
        void gen_var_assign(VarAssign assign);
        void gen_func_decl(FuncDecl decl);
    };
};

#endif // LUNA_IRGEN_HH
#if !defined(LUNA_IRGEN_HH)
#define LUNA_IRGEN_HH
#include "core.hh"
#include "ast.hh"
#include "diag.hh"

namespace luna{
    enum class IrType{
        PUSH, // Push an operand into the stack to prepare for any function calls
        CALL, // Call a functon
        FUNC_DECL, // Specifiy where a function begins
        LABEL_DECL, // This is for me to understand the IR a bit better and make it easier on myself when emiting the bytecode / source code
        STORE,
        DEFINE,
        RET,
    };
    class IRInst{
        public:
            IRInst(IrType type) :_type(type){}
            void add_operand(std::string operand) { operands.push_back(operand); }
            IrType _type;
            std::vector<std::string> operands;
    };
    class IR{
        public:
            IR(){}
            void add_inst(IRInst inst) { insts.push_back(inst); }
            void print(std::string prefix="");
            std::vector<IRInst> get_insts() { return insts; }
        private:
            std::vector<IRInst> insts;
    };
    class IrGen{
        public:
            // [NOTE TO SELF]: Please use this after calling the Sema not doing any checks in here and otherwise the assembler will complain
            IrGen(Ast ast, Diag diag) :_ast(ast), _diag(diag){}
            [[nodiscard]] IR Generate();
            [[nodiscard]] IR Generate_body();
        private:
            Ast _ast;
            Diag _diag;
    };
};

#endif // LUNA_IRGEN_HH

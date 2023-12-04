#if !defined(LUNA_CODEGEN_X86_64_HH)
#define LUNA_CODEGEN_X86_64_HH
#include "core.hh"
#include "irgen.hh"

namespace luna{
namespace CodeGen{
    // TODO: Support windows objects
    enum struct EmitType{
        X86_64, // NASM assembly
        ELF, // Executable object
        OBJ, // Linkable object
        LLVM, // LLVM 'assembly'
    };
    class CodeGen {
        public:
            CodeGen(IR ir, EmitType et) :_ir(ir), _emit_type(et){}
            void emit_nasm_assembly();
        private:
            EmitType _emit_type;
            IR _ir;
    };
};
};

#endif // LUNA_CODEGEN_X86_64_HH

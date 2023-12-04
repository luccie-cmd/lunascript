#include "codegen/x86_64.hh"

void luna::CodeGen::CodeGen::emit_nasm_assembly(){
    std::string emit = "";
    for(IRInst inst : _ir.get_insts()){
        switch(inst._type){
            case IrType::PUSH: {
                
            } break;
            case IrType::CALL: {} break;
            case IrType::FUNC_DECL: {} break;
            case IrType::LABEL_DECL: {} break;
            case IrType::STORE: {} break;
            case IrType::DEFINE: {} break;
            case IrType::RET: {} break;
        }
    }
}
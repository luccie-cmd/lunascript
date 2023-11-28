#include "irgen.hh"

luna::IR luna::IrGen::Generate(){
    std::vector<AstTypes> children = _ast.get_children();
    IR ret;
    for(AstTypes child : children){
        try {
            if (auto expr = std::get_if<Expr>(&child)) {
                switch(expr->get_type()){
                    case ExprType::CALL: {
                        for(Token t : expr->call_get_operands()){
                            IRInst push_inst(IrType::PUSH);
                            push_inst.add_operand(t._value);
                            ret.add_inst(push_inst);
                        }
                        IRInst inst(IrType::CALL);
                        inst.add_operand(expr->call_get_name());
                        ret.add_inst(inst);
                    } break;
                    default: {
                        _diag.ICE("No motivation to write errors found (It's in IRGen)\n");
                    } break;
                }
            } else if (auto funcDecl = std::get_if<FuncDecl>(&child)) {
                IRInst inst(IrType::FUNC_DECL);
                inst.add_operand(funcDecl->get_name()+":");
                ret.add_inst(inst);
                // Add the begining of the body decleration
                inst.operands = {};
                inst.add_operand(".bdy:");
                ret.add_inst(inst);
                IrGen gen(funcDecl->get_body(), _diag);
                IR body = gen.Generate();
                for(IRInst i : body.get_insts()){
                    ret.add_inst(i);
                }
                // Add the end of the body decleration
                inst.operands = {};
                inst.add_operand(".end_bdy:");
                ret.add_inst(inst);
            } else if (auto varAssign = std::get_if<VarAssign>(&child)) {
                IRInst inst(IrType::STORE);
                inst.add_operand(varAssign->get_name());
                inst.add_operand(varAssign->get_value<std::string>());
                ret.add_inst(inst);
            } else if (auto varDecl = std::get_if<VarDecl>(&child)) {
                IRInst inst(IrType::DEFINE);
                // I want to be able to also define lists and such
                inst.add_operand("var");
                inst.add_operand(varDecl->get_name());
                ret.add_inst(inst);
            } else {
                fmt::print("Unexpected variant type!\n");
            }
        } catch (const std::exception& ex) {
            _diag.Error("Exception occured: {}\n", ex.what());
        }
    }
    IRInst inst(IrType::RET);
    inst.add_operand("0");
    ret.add_inst(inst);
    return ret;
}

void luna::IR::print(std::string prefix){
    fmt::print("\nIR\n");
    for(IRInst inst : insts){
        switch(inst._type){
            case IrType::PUSH: {
                fmt::print("{}push {}\n", prefix, inst.operands.at(0));
            } break;
            case IrType::CALL: {
                fmt::print("{}call {}\n", prefix, inst.operands.at(0));
            } break;
            case IrType::FUNC_DECL:
            case IrType::LABEL_DECL: {
                fmt::print("{}{}\n", prefix, inst.operands.at(0));
            } break;
            case IrType::STORE: {
                fmt::print("{}store {} {}\n", prefix, inst.operands.at(0), inst.operands.at(1));
            } break;
            case IrType::DEFINE: {
                fmt::print("{}define {} {}\n", prefix, inst.operands.at(0), inst.operands.at(1));
            } break;
            case IrType::RET: {
                fmt::print("{}ret {}\n", prefix, inst.operands.at(0));
            } break;
        }
    }
}
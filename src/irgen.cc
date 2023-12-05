#include "irgen.hh"

luna::IR luna::IrGen::Generate(){
    std::vector<AstTypes> children = _ast.get_children();
    IR ret;
    for(AstTypes child : children){
        try {
            if (auto funcDecl = std::get_if<FuncDecl>(&child)) {
                IRInst inst(IrType::FUNC_DECL);
                inst.add_operand(funcDecl->get_name()+":");
                inst.add_operand(funcDecl->get_typehint_str());
                ret.add_inst(inst);
                IrGen gen(funcDecl->get_body(), _diag);
                IR body = gen.Generate_body();
                for(IRInst i : body.get_insts()){
                    ret.add_inst(i);
                }
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
    return ret;
}

luna::IR luna::IrGen::Generate_body(){
    std::vector<AstTypes> children = _ast.get_children();
    IR ret;
    bool returns = false;
    usz n = 0;
    for(usz i = 0; i < children.size(); ++i){
        AstTypes child = children.at(i);

        IRInst label_inst(IrType::LABEL_DECL);
        label_inst.add_operand(fmt::format(".bdy_{}:", i));
        ret.add_inst(label_inst);

        try {
            if (auto expr = std::get_if<Expr>(&child)) {
                        fmt::print("Call expr!\n");
                switch(expr->get_type()){
                    case ExprType::CALL: {
                        for(Token t : expr->call_get_operands()){
                            fmt::print("Token value = {}\n", t._value);
                            IRInst push_inst(IrType::PUSH);
                            // Other can be anything from an integer to a number
                            push_inst.add_operand(t._type == TokenType::STRING ? "string" : "other");
                            if(t._type != TokenType::STRING) {
                                fmt::print("No string\n");
                                push_inst.add_operand(t._value); 
                            }
                            else {
                                fmt::print("string\n");
                                ret.strings.push_back(t._value);
                                push_inst.add_operand(fmt::format("str_{}", ret.strings.size()-1));
                            }
                            ret.add_inst(push_inst);
                        }
                        IRInst inst(IrType::CALL);
                        inst.add_operand(expr->call_get_name());
                        ret.add_inst(inst);
                    } break;
                    default: {
                        _diag.ICE("Unhandled type found in IRGEN\n");
                    } break;
                }
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
            } else if (auto stmt = std::get_if<Stmt>(&child)) {
                switch(stmt->get_type()){
                    case StmtType::RETURN: {
                        IRInst inst(IrType::RET);
                        inst.add_operand(stmt->get_arguments().at(0)._value);
                        ret.add_inst(inst);
                        returns = true;
                    } break;
                }
            } else {
                fmt::print("[IRGEN]: Unexpected variant type!\n");
            }
        } catch (const std::exception& ex) {
            _diag.Error("Exception occured: {}\n", ex.what());
        }
        n = i;
    }
    if(!returns){
        IRInst label_inst(IrType::LABEL_DECL);
        label_inst.add_operand(fmt::format(".bdy_{}:", n+1));
        ret.add_inst(label_inst);
        IRInst inst(IrType::RET);
        inst.add_operand("0");
        ret.add_inst(inst);
    }
    return ret;
}

void luna::IR::print(std::string prefix){
    fmt::print("\nIR\n");
    for(usz i = 0; i < strings.size(); ++i){
        fmt::print("str_{}: {}\n", i, strings.at(i));
    }
    for(IRInst inst : insts){
        switch(inst._type){
            case IrType::PUSH: {
                fmt::print("{}push {} {}\n", prefix, inst.operands.at(0), inst.operands.at(1));
            } break;
            case IrType::CALL: {
                fmt::print("{}call {}\n", prefix, inst.operands.at(0));
            } break;
            case IrType::FUNC_DECL: {
                fmt::print("{}{} {}\n", prefix, inst.operands.at(0), inst.operands.at(1));
            } break;
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
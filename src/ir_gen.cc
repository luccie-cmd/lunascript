#include "ir_gen.hh"

luna::VarDecl luna::IrGen::find_var_decl_by_name(std::string name){
    for(std::pair<ArgType, std::string> var : ir.get_var_list()){
        if(var.second == name){
            VarDecl ret(name);
            ret.set_arg_type(var.first);
            return ret;
        }
    }
    _ctx.diag.Error("No variable exists with name `{}`\n", name);
    return VarDecl(name);
}

void luna::IrGen::gen_var_decl(luna::VarDecl decl){
    if(decl.get_name() == "__end__"){
        return;
    }
    IrInst inst(IrType::ALLOCA);

    inst.add_argument({decl.get_arg_type(), "1"});
    ir.add_var({decl.get_arg_type(), decl.get_name()});
    ir.add_inst(inst);
}

void luna::IrGen::gen_var_assign(VarAssign assign){
    IrInst inst(IrType::STORE);
    VarDecl decl = find_var_decl_by_name(assign.get_name());

    inst.add_argument({decl.get_arg_type(), assign.get_value()._value});
    inst.add_argument({decl.get_arg_type(), decl.get_name()});
    ir.add_inst(inst);
}

void luna::IrGen::gen_func_decl(FuncDecl decl){
    IrFunction func(decl.get_name(), &ir, decl.get_linkage(), decl.get_func_arguments());
    func.generate(_ctx, decl.get_body(), decl.get_func_arguments().size());
    ir.add_func(func);
}

luna::Ir luna::IrGen::get_ir(){
    for(AstTypes child : _ast.get_children()){
        switch((AstType)(child.index()+1)){
            case AstType::VAR_DECL: {
                gen_var_decl(*std::get<std::shared_ptr<VarDecl>>(child).get());
            } break;
            case AstType::VAR_ASSIGN: {
                gen_var_assign(*std::get<std::shared_ptr<VarAssign>>(child).get());
            } break;
            case AstType::VAR_DECLASSIGN: {
                VarDeclAssign decl_assign = *std::get<std::shared_ptr<VarDeclAssign>>(child).get();
                VarDecl decl(decl_assign.get_name());
                decl.set_arg_type(decl_assign.get_arg_type());

                VarAssign assign(decl_assign.get_name(), decl_assign.get_value());

                gen_var_decl(decl);
                gen_var_assign(assign);
            } break;
            case AstType::FUNC_DECL: {
                FuncDecl func_decl = *std::get<std::shared_ptr<FuncDecl>>(child).get();
                gen_func_decl(func_decl);
            } break;
            case AstType::STMT:
            default: {
                _ctx.diag.ICE("Unable to generate IR for type {}\n", child.index()+1);
            } break;
        }
    }
    return ir;
}

void luna::IrFunction::generate(Context ctx, BlockStmt block_body, usz args_len){
    
}
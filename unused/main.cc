if(sema.is_build()){
    Interpreter::InterPreter Inter(ctx, ast);
    Inter.interpret_ast();
    if(verbose){
        fmt::print("Project Name = {}\n", Inter.get_prt_name());
        fmt::print("Build directory = {}\n", Inter.get_dst());
        fmt::print("Version = {}\n", Inter.get_version());
        fmt::print("Language std = {}\n", Inter.get_language_std());
        fmt::print("Libs path = {}\n", Inter.get_libs_path());
        fmt::print("Files\n");
        for(std::string s : Inter.get_files()){
            fmt::print("{}\n", s);
        }
        fmt::print("Languages\n", Inter.get_language());
        fmt::print("Variables\n");
        for (const auto& var : Inter.get_variables()) {
            fmt::print("{} = {}\n", var.first, var.second);
        }
        fmt::print("Functions\n");
        for(const auto& func : Inter.get_functions()){
            fmt::print("Name = {}\n", func.first);
            fmt::print("Body\n");
            Ast body;
            for(AstTypes body_ast : func.second.get_body()){
                body.add_child(body_ast);
            }
            body.print();
        }
    }
}
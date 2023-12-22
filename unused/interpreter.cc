#include "interpreter.hh"

bool luna::Interpreter::InterPreter::directoryExists(const std::string& path) {
    return fs::is_directory(path);
}

std::string luna::Interpreter::InterPreter::handle_f_string(std::string string){
    std::string ret_buffer;
    bool is_f_string = false;
    std::string f_string_buffer;
    for(char c : string){
        if(c == '{'){
            is_f_string = true;
        } else{
            if(is_f_string){
                if(c == '}'){
                    is_f_string = false;
                    if(f_string_buffer == "DESTINATION"){
                        ret_buffer += dst + '/' + version;
                    }
                } else{
                    f_string_buffer += c;
                }
            } else{
                ret_buffer += c;
            }
        }
    }
    return ret_buffer;
}

void luna::Interpreter::InterPreter::interpret_ast(){
    for(AstTypes child : ast){
        AstType type = (AstType)(child.index()+1);
        switch(type){
            case AstType::VAR_DECL: {
                std::shared_ptr<VarDecl> decl = std::get<std::shared_ptr<VarDecl>>(child);
                if(decl.get()->get_name() == "__end__"){
                    continue;
                }
                variables.push_back(std::make_pair(std::string(decl.get()->get_name().c_str()), "0"));
            } break;
            case AstType::VAR_DECLASSIGN: {
                auto assign = std::get<std::shared_ptr<VarDeclAssign>>(child);
                if(assign.get()->get_name() == "__end__"){
                    continue;
                }
                std::string name = assign.get()->get_name();
                variables.push_back(std::make_pair(name, "0"));
                for (auto& var : variables) {
                    const std::string& var_name = var.first;
                    std::string& value = var.second;
                    
                    if (var_name == name) {
                        value = assign.get()->get_value()._value;
                    }
                }
            } break;
            case AstType::VAR_ASSIGN: {
                auto assign = std::get<std::shared_ptr<VarAssign>>(child);
                VarAssign a = *assign.get();
                std::string name = a.get_name();
                if(name == "project_name"){
                    prt_name = a.get_value()._value;
                } else if(name == "destination_dir"){
                    dst = a.get_value()._value;
                    system(fmt::format("mkdir -p {}", dst).c_str());
                    system(fmt::format("mkdir -p {}/{}\n", dst, version).c_str());
                } else if(name == "project_version"){
                    version = a.get_value()._value;
                } else if(name == "libs_path"){
                    libs_path = handle_f_string(a.get_value()._value);
                } else{
                    for (auto& var : variables) {
                        const std::string& var_name = var.first;
                        std::string& value = var.second;
                        if (var_name == a.get_name()) {
                            value = a.get_value()._value;
                        }
                    }
                }
            } break;
            case AstType::FUNC_DECL: {
                auto func = std::get<std::shared_ptr<FuncDecl>>(child);
                FuncDecl decl = *func.get();
                functions.push_back(std::make_pair(decl.get_name(), decl.get_body()));
            } break;
            case AstType::EXPR: {
                auto expr_types = std::get<ExprTypes>(child);
                ExprType idx = (ExprType)expr_types.index();
                switch(idx){
                    case ExprType::CALL: {
                        auto call_expr = std::get<std::shared_ptr<CallExpr>>(expr_types);
                        if(call_expr->get_name() == "set"){
                            std::vector<Token> ops = call_expr->get_operands();
                            std::vector<std::string> ops_strings;
                            for(Token op : ops){
                                ops_strings.push_back(op._value);
                            }
                            if(ops_strings.at(0) == "Languages"){
                                language = ops_strings.at(1);
                            } else if(ops_strings.at(0) == "CXX_Language_Standard"){
                                if(language != "CXX"){
                                    ctx.diag.Error("Cannot specify CXX language if it isn't the currently used language!\n");
                                }
                                language_std = fmt::format("-std=c++{}", ops_strings.at(1));
                            } else if(ops_strings.at(0) == "files"){
                                // TODO: Check if last argument is true and if it is go trough every child directory of it and add those files
                                for(usz i = 1; i < call_expr->get_operands().size()-(call_expr->get_operands().at(call_expr->get_operands().size()-1)._value == "true" ? 1 : 0); ++i){
                                    files.push_back(call_expr->get_operands().at(i)._value);
                                }
                            } else if(ops_strings.at(0) == "arguments"){
                                arguments = ops_strings;
                                reverse_vector_in_place(arguments);
                                vector_pop_back(arguments);
                                reverse_vector_in_place(arguments);
                            }
                        } else if(call_expr->get_name() == "print"){
                            std::string print_buffer;
                            for(Token op : call_expr->get_operands()){
                                bool is_escape = false;
                                for(char c : op._value){
                                    if(c == '\\'){
                                        is_escape = true;
                                    } else{
                                        if(is_escape){
                                            switch(c){
                                                case 'n': print_buffer += '\n'; break;
                                                case 'b': print_buffer += '\b'; break;
                                                case 'r': print_buffer += '\r'; break;
                                                case 'v': print_buffer += '\v'; break;
                                                case '\\': print_buffer += '\\'; break;
                                            }
                                        } else{
                                            print_buffer += c;
                                        }
                                        is_escape = false;
                                    }
                                }
                            }
                            fmt::print("{}", print_buffer);
                        } else if(call_expr->get_name() == "add_git_repo"){
                            std::string link = call_expr->get_operands().at(0)._value;
                            std::string i_dst = call_expr->get_operands().at(1)._value;
                            std::string dst_buffer = handle_f_string(i_dst);
                            if(!directoryExists(dst_buffer)){
                                while(1) {
                                    ctx.diag.Info("Cloning {}\n", link);
                                    int result = system(fmt::format("git clone -q {} {}", link, dst_buffer).c_str());
                                    if(result == 0){
                                        break;
                                    }
                                    ctx.diag.Info("Trying to clone `{}` again...\n", link);
                                }
                            }
                        } else if(call_expr->get_name() == "add_library"){
                            std::string path = libs_path+'/'+call_expr->get_operands().at(0)._value;
                            if(!directoryExists(path)){
                                ctx.diag.Info("Searched in {}\n", path);
                                ctx.diag.Error("Invalid library adding!\n");
                            }
                            system(fmt::format("mkdir -p {}-build\n", path).c_str());
                            system(fmt::format("mkdir -p {}/LunaCache/{}/", dst, call_expr->get_operands().at(0)._value).c_str());
                            for (const auto& entry : fs::directory_iterator(path+"/src")){
                                if(system(fmt::format("stat {}/LunaCache/{}/{} > /dev/null 2>&1", dst, call_expr->get_operands().at(0)._value, entry.path().filename().c_str()).c_str()) == 0){
                                    // Check if the file is the same
                                    if(system(fmt::format("diff {0}/{1} {2}/LunaCache/fmt/{1}  > /dev/null", path+"/src", entry.path().filename().c_str(), dst).c_str()) != 0){
                                        ctx.diag.Info("Compiling CXX File {}/{}\n", path, entry.path().filename().c_str());
                                        system(fmt::format("g++ -c -o {0}-build/{1}.o {0}/src/{1} -O2 -fmodules-ts -I{0}/include -std=c++23", path, entry.path().filename().c_str()).c_str());
                                        system(fmt::format("cp {0}/src/{1} {2}/LunaCache/{3}/{1}", 
                                            path,
                                            entry.path().filename().c_str(),
                                            dst,
                                            call_expr->get_operands().at(0)._value
                                        ).c_str());
                                    }
                                } else{
                                    // Just build it no questions asked
                                    ctx.diag.Info("Compiling CXX File {}/{}\n", path, entry.path().filename().c_str());
                                    system(fmt::format("g++ -c -o {0}-build/{1}.o {0}/src/{1} -O2 -fmodules-ts -I{0}/include -std=c++23", path, entry.path().filename().c_str()).c_str());
                                    system(fmt::format("cp {0}/src/{1} {2}/LunaCache/{3}/{1}", 
                                        path,
                                        entry.path().filename().c_str(),
                                        dst,
                                        call_expr->get_operands().at(0)._value
                                    ).c_str());
                                }
                            }
                            ctx.diag.Info("Linking lib{}\n", call_expr->get_operands().at(0)._value);
                            system(fmt::format("ar rcs {0}-build/lib{1}.a {0}-build/*.o", path, call_expr->get_operands().at(0)._value).c_str());
                        } else if(call_expr->get_name() == "build"){
                            if(language == "CXX"){
                                for(const std::string path : files){
                                    fmt::print("File path {}\n", path);
                                    system(fmt::format("mkdir -p {}/LunaCache/{}", dst, path).c_str());
                                    for (const auto& entry : fs::directory_iterator(path)){
                                        if(system(fmt::format("stat {}/LunaCache/{}/{} > /dev/null 2>&1", dst, path, entry.path().filename().c_str()).c_str()) == 0){
                                            // Check if the file is the same
                                            if(system(fmt::format("diff {0}/{1} {2}/LunaCache/fmt/{1}  > /dev/null", path, entry.path().filename().c_str(), dst).c_str()) != 0){
                                                ctx.diag.Info("Compiling CXX File {}/{}\n", path, entry.path().filename().c_str());
                                                system(fmt::format("g++ -c -o {0}/{1}.o {0}/{1} -O2 -fmodules-ts -I{0}/include -std=c++23", path, entry.path().filename().c_str()).c_str());
                                                system(fmt::format("cp {0}/{1} {2}/LunaCache/{3}/{1}", 
                                                    path,
                                                    entry.path().filename().c_str(),
                                                    dst,
                                                    path
                                                ).c_str());
                                            }
                                        } else{
                                            // Just build it no questions asked
                                            ctx.diag.Info("Compiling CXX File {}/{}\n", path, entry.path().filename().c_str());
                                            system(fmt::format("g++ -c -o {0}-build/{1}.o {0}/{1} -O2 -fmodules-ts -I{0}/include -std=c++23", path, entry.path().filename().c_str()).c_str());
                                            system(fmt::format("cp {0}/{1} {2}/LunaCache/{3}/{1}", 
                                                path,
                                                entry.path().filename().c_str(),
                                                dst,
                                                path
                                            ).c_str());
                                        }
                                    }
                                }
                            }
                        } else if(call_expr->get_name() == "include_directory"){
                            arguments.push_back(handle_f_string(call_expr->get_operands().at(0)._value));
                        } else{
                            for(auto& func : functions){
                                if(func.first == call_expr->get_name()){
                                    Ast new_ast;
                                    for(auto child : func.second.get_body()){
                                        new_ast.add_child(child);
                                        new_ast.populate_scope();
                                    }
                                    InterPreter inT(ctx, new_ast);
                                    // Add all things accumulated so far
                                    inT.setup_func_call(*this);
                                    inT.interpret_ast();
                                    // Add the things found in the function into the current interpreter
                                    setup_func_call(inT);
                                }
                            }
                        }
                    } break;
                }
            } break;
        }
    }
}
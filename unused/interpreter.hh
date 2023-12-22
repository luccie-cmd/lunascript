#if !defined(LUNA_INTERPRETER_HH)
#define LUNA_INTERPRETER_HH
#include "core.hh"
#include "ast.hh"

namespace luna{
namespace Interpreter{
    class InterPreter{
        private:
            std::string prt_name = "None";
            std::string dst = "build";
            std::string version = "0";
            std::string libs_path;
            std::vector<std::pair<std::string, std::string>> variables;
            std::vector<std::pair<std::string, BlockStmt>> functions;
            std::vector<std::string> files;
            std::string language_std, language;
            std::vector<std::string> arguments;
            Context ctx;
            std::vector<AstTypes> ast;
            bool directoryExists(const std::string& path);
            std::string handle_f_string(std::string string);
        public:
            InterPreter(Context _ctx, Ast _ast) :ctx(ctx){
                ast = _ast.get_children();
            }
            std::string get_prt_name() { return prt_name; };
            std::string get_dst() { return dst; };
            std::string get_version() { return version; };
            std::string get_libs_path() { return libs_path; };
            std::vector<std::pair<std::string, std::string>> get_variables() { return variables; };
            std::vector<std::pair<std::string, BlockStmt>> get_functions() { return functions; };
            std::vector<std::string> get_files() { return files; };
            std::string get_language_std() { return language_std; };
            std::string get_language() { return language; };
            std::vector<std::string> get_arguments() { return arguments; };
            void setup_func_call(InterPreter inter){
                this->prt_name = inter.prt_name;
                this->dst = inter.dst;
                this->version = inter.version;
                this->libs_path = inter.libs_path;
                this->variables = inter.variables;
                this->functions = inter.functions;
                this->files = inter.files;
                this->language = inter.language;
                this->language_std = inter.language_std;
                this->arguments = inter.arguments;
            }
            void interpret_ast();
    };
};
};


#endif // LUNA_INTERPRETER_HH

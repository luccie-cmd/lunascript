#if !defined(LUNA_SEMA_HH)
#define LUNA_SEMA_HH
#include "core.hh"
#include "diag.hh"
#include "ast.hh"
#include "token.hh"

namespace luna{
    class SeMa{
        public:
            SeMa(Ast ast, Diag diag) :_ast(ast), _diag(diag){}
            void check(bool is_body=false);
            void check_var_decl(VarDecl child);
            void check_var_assign(VarAssign assign);
            void check_func_decl(FuncDecl decl);
            void check_expr(Expr expr);
            void add_declared_functions(std::vector<std::pair<std::string, std::pair<TypeHint, usz>>> functions){
                for(std::pair<std::string, std::pair<TypeHint, usz>> f : functions){
                    declared_functions.push_back(f);
                }
            }
        private:
            std::vector<std::string> varDecl_names;
            std::vector<std::pair<std::string, std::pair<TypeHint, usz>>> declared_functions;
            std::vector<std::pair<std::string, std::vector<Token>>> called_functions;
            std::vector<Ast> bodies;
            Ast _ast;
            Diag _diag;
    };
};

#endif // LUNA_SEMA_HH

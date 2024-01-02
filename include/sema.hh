#if !defined(LUNA_SEMA_HH)
#define LUNA_SEMA_HH
#include "core.hh"
#include "ast.hh"

namespace luna{
    // A container for storing things used in all the Sema Funcitons
    struct SemaContext {
        std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> declared_functions;
        std::vector<std::string> declared_variables;
        std::vector<AstTypes> current_func_body; // This is used by the Find_var_decl function
        std::string defined_function; // Function wherein we currently are
        bool build, is_body; // For enabeling the build system
    };
    class Sema{
        private:
            Ast _ast;
            Context _ctx;
            void analyse_blockStmt(SemaContext sctx);
            SemaContext analyse_astTypes(AstTypes type, SemaContext _sctx);
        public:
            Sema(Ast ast, Context ctx) :_ast(ast), _ctx(ctx){}
            void analyse();
    };
};

#endif // LUNA_SEMA_HH

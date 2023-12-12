#if !defined(LUNA_SEMA_HH)
#define LUNA_SEMA_HH
#include "core.hh"
#include "ast.hh"

namespace luna{
    // A container for storing things used in all the Sema Funcitons
    struct SemaContext {
        std::vector<std::string> declared_variables, declared_functions;
        bool build; // For enabeling the build system
    };
    class Sema{
        private:
            Ast _ast;
            Context _ctx;
            SemaContext _sctx;
            void analyse_blockStmt(BlockStmt stmt);
            void analyse_astTypes(AstTypes type);
            void setup_sema_build();
        public:
            Sema(Ast ast, Context ctx) :_ast(ast), _ctx(ctx){}
            void analyse();
            bool is_build(){ return _sctx.build; }
    };
};

#endif // LUNA_SEMA_HH

#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "parser.hh"
#include "diag.hh"
#include "sema.hh"
#include "irgen.hh"
#include "backend/codegen/x86_64.hh"

using namespace command_line_options;

using options = clopts<
    flag<"--print-ast", "Print the AST">,
    flag<"--print-ir", "Print the IR">,
    // NOTE: Do proper english on this
    flag<"--no-sema", "Disable the Analasis checks">,
    option<"-f", "What format to emit code in (default: nasm)", 
        values<"nasm", 
               "obj", "elf", 
               "llvm"
        >
    >,
    positional<"file", "The file whose contents should be read and compiled", file<>, /*required=*/true>,
    help<>
>;

using namespace luna;

int main(int argc, char** argv){
    auto opts = options::parse(argc, argv);
    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    file_contents += '\n';
    bool print_ast = opts.get<"--print-ast">();
    bool print_ir = opts.get<"--print-ir">();
    bool sema_enable = opts.get<"--no-sema">() ? false : true;
    auto emit_type = opts.get_or<"-f">("nasm");
    Diag diag;
    if(!sema_enable) diag.Warning("Disableing Sema can have some mistakes!\n");
    //       color  exit
    diag.init(true, true);
    Lexer lexer(std::string(file_name), file_contents, diag);
    Parser parser(lexer, diag);
    Ast ast = parser.nodes();
    SeMa sema(ast, diag);
    if(sema_enable) sema.check();
    if(print_ast) ast.print();
    IrGen irgen(ast, diag);
    IR ir = irgen.Generate();
    if(print_ir) ir.print();
    return 0;
}
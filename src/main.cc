#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "parser.hh"
#include "diag.hh"
#include "sema.hh"

using namespace command_line_options;

using options = clopts<
    flag<"--print-ast", "Print the AST">,
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
    Diag diag;
    //       color  exit
    diag.init(true, true);
    Lexer lexer(std::string(file_name), file_contents, diag);
    Parser parser(lexer, diag);
    Ast ast = parser.nodes();
    SeMa sema(ast, diag);
    sema.check();
    if(print_ast) ast.print();
    return 0;
}
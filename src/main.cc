#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "diag.hh"
#include "parser.hh"
#include "sema.hh"

using namespace command_line_options;

using options = clopts<
    flag<"--no-color", "Disables the colored printing">,
    flag<"--ast", "Prints the AST">,
    positional<"file", "Path to files that should be compiled", file<>, true>,
    help<>>;

using namespace luna;

int main(int argc, char **argv)
{
    auto opts = options::parse(argc, argv);

    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    file_contents.push_back('\n');
    // Might be confusing but this just disabeles the color if the flag is present
    bool color = opts.get<"--no-color">() ? false : true;
    bool print_ast = opts.get<"--ast">();
    Context ctx(color, exit);
    Lexer lexer(ctx, std::string(file_name), file_contents);
    Parser parser(ctx, lexer);
    parser.parse();
    Ast ast = parser.get_ast();
    Sema sema(ast, ctx);
    sema.analyse();
    if(print_ast) ast.print();
    return 0;
}
#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "diag.hh"
#include "parser.hh"

using namespace command_line_options;

using options = clopts<
    option<"--no-color", "Disables the colored printing">,
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
    Context ctx(color, true);
    Lexer lexer(ctx, std::string(file_name), file_contents);
    Parser parser(ctx, lexer);
    parser.parse();
    Ast ast = parser.get_ast();
    ast.print();
    return 0;
}
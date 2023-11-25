#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "parser.hh"

using namespace command_line_options;

using options = clopts<
    flag<"--print-ast", "Print the AST">,
    flag<"--no-errors", "Disable all the error exits (does not disable colors)">,
    positional<"file", "The file whose contents should be read and compiled", file<>, /*required=*/true>,
    help<>
>;

using namespace luna;

int main(int argc, char** argv){
    auto opts = options::parse(argc, argv);
    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    Lexer lexer(std::string(file_name), file_contents);
    Parser parser(lexer);
    Ast ast = parser.nodes();
    ast.print();
    return 0;
}
#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "parser.hh"

using namespace command_line_options;

using options = clopts<
    positional<"file", "The file whose contents should be read and compiled", file<>, /*required=*/true>,
    help<>
>;

int main(int argc, char** argv){
    auto opts = options::parse(argc, argv);
    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    luna::Lexer lexer(std::string(file_name), file_contents);
    luna::Parser parser(lexer);
    luna::Ast ast = parser.nodes();
    return 0;
}
#include <fmt/core.h>
#include "deps/clopts.hh"
#include "core.hh"
#include "lexer.hh"
#include "diag.hh"

using namespace command_line_options;

using options = clopts<
    positional<"file", "The file whose contents should be read and compiled", file<>, /*required=*/true>,
    help<>
>;

using namespace luna;

int main(int argc, char** argv){
    auto opts = options::parse(argc, argv);
    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    file_contents += '\n';
    Diag diag;
    //       color  exit
    diag.init(true, false);
    Lexer lexer(std::string(file_name), file_contents, diag);
    std::vector<Token> tokens = lexer.lex();
    for(Token t : tokens){
        t.print();
    }
    return 0;
}
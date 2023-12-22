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
    flag<"-v", "Enables verbose option">,
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
    bool verbose = opts.get<"-v">();
    Context ctx(color, true, verbose);

    if(verbose){
        fmt::print("File name: {}\n", file_name.c_str());
    }

    Lexer verify_tokens(ctx, std::string(file_name), file_contents);
    std::vector<Token> tokens = verify_tokens.lex();
    for(Token t : tokens){
        switch(t._type){
            case TokenType::ID: {
                if(isdigit(t._value.at(0))){
                    ctx.diag.ICE("{}: Lexer failed to recognize this token as a number\n", t.loc.to_str());
                }
            } break;
            case TokenType::STRING: {
                bool escape = false;
                std::string escape_chars = "nbtv\\";
                for(char c : t._value){
                    if(escape){
                        escape = false;
                        if(!escape_chars.contains(c)){
                            ctx.diag.Error("Invalid escape character {}!\n", c);
                        }
                    }
                    else if(c == '\\'){
                        escape = true;
                    }
                }
            } break;
            case TokenType::NUMBER: {
                for(char c : t._value){
                    if(!isdigit(c)){
                        ctx.diag.Error("{} Invalid numeric literal: {}\n", t.loc.to_str(), c);
                    }
                }
            } break;
            case TokenType::FLOAT: {
                for(char c : t._value){
                    // We only do this to make build system happy (the .)
                    if(!isdigit(c) && c != '.'){
                        ctx.diag.Error("{} Invalid float literal: {}\n", t.loc.to_str(), c);
                    }
                }
            } break;
        }
    }
    
    Lexer lexer(ctx, std::string(file_name), file_contents);
    Parser parser(ctx, lexer);
    parser.parse();
    Ast ast = parser.get_ast();
    Sema sema(ast, ctx);
    sema.analyse();
    if(print_ast) ast.print();
    return 0;
}
#include "lexer.hh"
#define COMMENT_SYMBOL '#'

namespace luna{
    
void Lexer::advance(){
    if(idx < _contents.size()){
        idx+=1;
        _c = _contents.at(idx);
        if(_c == '\n'){
            loc.col = 0;
            loc.row++;
        } else if(_c == '\t'){
            loc.col+=4;
        } else{
            loc.col++;
        }
    }
}

void Lexer::skip_whitespace(){
    while(isskip(_c) && idx < _contents.size()-1){
        advance();
    }
}


Token Lexer::next_token(){
    skip_whitespace();
    // first off check for EOF
    if(idx >= _contents.size()-1) return Token(TokenType::TT_EOF, "\0");
    // Next check for any recurring or single comments
    while(_c == COMMENT_SYMBOL && idx < _contents.size()-1){
        while(_c != '\n' && idx < _contents.size()-1) advance();
        skip_whitespace();
    }
    // Another check for EOF
    skip_whitespace();
    if(idx >= _contents.size()-1) return Token(TokenType::TT_EOF, "\0");

// include the hardcoded tokens
#include "tokens.decl"
    // Find any single tokens
    for(Token t : hardcoded_tokens){
        if(_c == *t._value.c_str()){
            advance();
            return t;
        }
    }

    // Check for any ids / numbers
    if(isalnum(_c)){
        std::string buffer;
        buffer.push_back(_c);
        advance();
        while(isalnum(_c)){
            buffer.push_back(_c);
            advance();
        }
        return Token(TokenType::ID, buffer);
    }

    // check for strings
    if(_c == '"'){
        std::string buffer;
        advance();
        while(_c != '"'){
            buffer.push_back(_c);
            advance();
        }
        // skip the next " token
        advance();
        return Token(TokenType::STRING, buffer);
    }

    // if all fails exit and print the location
    fmt::print("{}:{}:{}: ERROR: Invalid token {}\n", loc.file, loc.row, loc.col, _c);
    exit(1);
}

std::vector<Token> Lexer::lex(){
    std::vector<Token> ret;
    Token next = next_token();
    ret.push_back(next);
    while(next._type != TokenType::TT_EOF){
        next = next_token();
        ret.push_back(next);
    }
    return ret;
}

} // namespace luna

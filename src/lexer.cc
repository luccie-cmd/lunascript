#include "lexer.hh"
#define COMMENT_SYMBOL '#'

namespace luna
{

    void Lexer::advance()
    {
        if (idx < _contents.size())
        {
            idx += 1;
            _c = _contents.at(idx);
            loc.update(_c);
        }
    }

    void Lexer::skip_whitespace()
    {
        while (isskip(_c) && idx < _contents.size() - 1)
        {
            advance();
        }
    }

    Token Lexer::next_token()
    {
        skip_whitespace();
        // first off check for EOF
        if (idx >= _contents.size() - 1)
            return Token(TokenType::TT_EOF, "\0", loc);
        // Next check for any recurring or single comments
        while (_c == COMMENT_SYMBOL && idx < _contents.size() - 1)
        {
            while (_c != '\n' && idx < _contents.size() - 1)
                advance();
            skip_whitespace();
            // loc.next_row();
        }
        // Another check for EOF
        skip_whitespace();
        if (idx >= _contents.size() - 1)
            return Token(TokenType::TT_EOF, "\0", loc);

        switch (_c)
        {
        case '.':
        case ',':
        case ';':
        case ':':
        case '(':
        case ')':
        case '{':
        case '}':
        {
            char pref = _c;
            advance();
            std::string value;
            value.push_back(pref);
            return Token(static_cast<TokenType>(pref), value, loc);
        }
        break;

        // TODO: Handle multiple tokens
        case '=':
        {
            char pref = _c;
            advance();
            std::string value;
            value.push_back(pref);
            return Token(static_cast<TokenType>(pref), value, loc);
        }
        break;
        }

        // Check for any ids / numbers
        if (isalnum(_c) || _c == '_')
        {
            std::string buffer;
            buffer.push_back(_c);
            advance();
            while (isalnum(_c) || _c == '_' || _c == '.')
            {
                buffer.push_back(_c);
                advance();
            }
            if (isdigit(buffer.at(0)))
            {
                if(buffer.contains('.')){
                    return Token(TokenType::FLOAT, buffer, loc);
                }
                return Token(TokenType::NUMBER, buffer, loc);
            }
            return Token(TokenType::ID, buffer, loc);
        }

        // check for strings
        if (_c == '"')
        {
            std::string buffer;
            advance();
            while (_c != '"')
            {
                buffer.push_back(_c);
                advance();
            }
            // skip the next " token
            advance();
            return Token(TokenType::STRING, buffer, loc);
        }

        // if all fails exit and print the location
        _ctx.diag.Error("{}: ERROR: Invalid token {}\n", loc.to_str(), _c);
        // Make the compiler shut up
        std::exit(_ctx.diag.Exit_Code());
    }

    std::vector<Token> Lexer::lex()
    {
        std::vector<Token> ret;
        Token next = next_token();
        ret.push_back(next);
        while (next._type != TokenType::TT_EOF)
        {
            next = next_token();
            ret.push_back(next);
        }
        return ret;
    }

    Token Lexer::peek(){
        usz curr_idx = idx;
        Token temp = next_token();
        idx = curr_idx;
        _c = _contents.at(idx);
        return temp;
    }

} // namespace luna

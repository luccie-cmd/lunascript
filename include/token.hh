#if !defined(LUNA_TOKEN_HH)
#define LUNA_TOKEN_HH
#include "core.hh"

namespace luna
{
    // Basic tokentypes
    enum struct TokenType : int
    {
        TT_EOF,
        ID = 1,
        STRING,
        NUMBER,
        FLOAT,
        DOT = '.',
        OPEN_PAREN = '(',
        CLOSE_PAREN = ')',
        OPEN_CURLY = '{',
        CLOSE_CURLY = '}',
        SEMICOLON = ';',
        COLON = ':',
        COMMA = ',',
        EQUAL = '=',
    };
    // use a struct so no getter or setter functions have to be used
    struct Token
    {
    public:
        /// @brief This is the delceration for a token
        /// @param type Specifies the tokentype to be used in parsing
        /// @param value Specific value of the token
        Token(TokenType type, std::string value, Loc _loc) : _type(type), _value(value), loc(_loc) {}
        // Stores the tokentype declared above this decleration
        TokenType _type;
        // Why not use std::variant or std::optional you may ask
        // this is because if the we don't reccognize a token it'll not work
        // we could make it optional but i can't be fucked to do so
        // stores the data of the token
        std::string _value;
        // simple print function that prints that type as an int and the data
        void print()
        {
            fmt::print("{}: `{}`\n", static_cast<int>(_type), _value);
        }
        // Stores the location of the token
        Loc loc;
    };
};

#endif // LUNA_TOKEN_HH

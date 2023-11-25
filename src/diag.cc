#include "diag.hh"

namespace luna{
    void Diag::init(bool color, bool exit){
        use_color = color;
        exit_on_error = exit;
    }
    std::string Diag::string_from_color(Color c){
        if(!use_color){
            return "";
        }
        switch (c) {
            case Color::Reset: return "\033[m";
            case Color::Red: return "\033[31m";
            case Color::Green: return "\033[32m";
            case Color::Yellow: return "\033[33m";
            case Color::Blue: return "\033[34m";
            case Color::Magenta: return "\033[35m";
            case Color::Cyan: return "\033[36m";
            case Color::White: return "\033[m\033[37m";
        }
        return string_from_color(Color::Reset);
    }
};
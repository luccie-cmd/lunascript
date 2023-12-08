#if !defined(LUNA_DIAG_HH)
#define LUNA_DIAG_HH
#include <fmt/core.h>

namespace luna
{
    class Diag
    {
    private:
        // Define some basic ANSI colors
        enum Color
        {
            Reset = 0,
            Red = 31,
            Green = 32,
            Yellow = 33,
            Blue = 34,
            Magenta = 35,
            Cyan = 36,
            White = 37,
        };
        // Specify if we need to use colors
        bool use_color = true;
        // Specify if we exit when error or ICE is called
        bool exit_on_error = true;

        // Colors
        Color info_color = Color::Reset;
        Color note_color = Color::Reset;
        Color warning_color = Color::Magenta;
        Color error_color = Color::Red;

        // Color helper
        std::string string_from_color(Color c);

    public:
        void init(bool color = true, bool exit = true);
        template <typename... Args>
        void Info(fmt::format_string<Args...> fmt, Args &&...args)
        {
            fmt::print("{}INFO: {}{}", string_from_color(info_color), fmt::format(fmt, std::forward<Args>(args)...), string_from_color(Color::Reset));
        }
        template <typename... Args>
        void Note(fmt::format_string<Args...> fmt, Args &&...args)
        {
            fmt::print("{}NOTE: {}{}", string_from_color(note_color), fmt::format(fmt, std::forward<Args>(args)...), string_from_color(Color::Reset));
        }
        template <typename... Args>
        void Warning(fmt::format_string<Args...> fmt, Args &&...args)
        {
            fmt::print("{}WARNING: {}{}", string_from_color(warning_color), fmt::format(fmt, std::forward<Args>(args)...), string_from_color(Color::Reset));
        }
        template <typename... Args>
        void Error(fmt::format_string<Args...> fmt, Args &&...args)
        {
            fmt::print("{}ERROR: {}{}", string_from_color(error_color), fmt::format(fmt, std::forward<Args>(args)...), string_from_color(Color::Reset));
            if (exit_on_error)
                std::exit(1);
        }
        template <typename... Args>
        void ICE(fmt::format_string<Args...> fmt, Args &&...args)
        {
            fmt::print("{}Internal Compiler Error: {}{}", string_from_color(error_color), fmt::format(fmt, std::forward<Args>(args)...), string_from_color(Color::Reset));
            if (exit_on_error)
                std::exit(1);
        }
    };
};

#endif // LUNA_DIAG_HH

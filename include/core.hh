#if !defined(LUNA_CORE_HH)
#define LUNA_CORE_HH

#include <fmt/core.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <string>

#include "diag.hh"

namespace luna
{
    // Name some basic types so that I don't have to write them every time
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using usz = std::size_t;

    // Some vector expansions
    template <typename T>
    void reverse_vector_in_place(std::vector<T> &list)
    {
        std::reverse(list.begin(), list.end());
    }
    template <typename T>
    T vector_pop_back(std::vector<T> &list)
    {
        if (!list.empty())
        {
            T ret = list.back();
            list.pop_back();
            return ret;
        }
        else
        {
            fmt::print("Cannot pop from an empty list!\n");
            std::exit(1);
        }
    }
    template<typename T>
    // Appends the second one after the first one
    // first-second
    std::vector<T> copy_vector(std::vector<T> first, std::vector<T> second){
        std::vector<T> ret = first;
        for(T elem : second){
            ret.push_back(elem);
        }
        return ret;
    }

    // A location class for the tokens and AST nodes
    // Also this can't be a struct because of the functions
    class Loc
    {
    public:
        std::string _file;
        usz _row = 1, _col = 1;
        Loc() : _file("None") {}
        Loc(std::string file) : _file(file) {}
        void next_row()
        {
            _row++;
            _col = 1;
        }
        std::string to_str();
        void update(int c);
    };

    bool string_is_int(std::string string);

    class Context
    {
    public:
        bool verbose;
        Diag diag;
        Context(bool color, bool exit, bool _verbose) :verbose(_verbose)
        {
            diag.init(color, exit);
        }
    };
};

#endif // LUNA_CORE_HH
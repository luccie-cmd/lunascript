#if !defined(LUNA_CORE_HH)
#define LUNA_CORE_HH

#include <fmt/core.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <string>

namespace luna{
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using usz = std::size_t;
    class Loc{
        private:
            std::string _file;
            usz _row = 1, _col = 1;
        public:
            Loc() :_file("None"){}
            Loc(std::string file) :_file(file){}
            void next_row(){ _row++; _col = 1; }
            std::string to_str();
            void update(int c);
    };
};

#endif // LUNA_CORE_HH
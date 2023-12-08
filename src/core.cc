// For any core defined functions (Loc)
#include "core.hh"

std::string luna::Loc::to_str()
{
    return std::string(fmt::format("{}:{}:{}", _file, _row, _col));
}

void luna::Loc::update(int c)
{
    if (c == '\n')
    {
        _col = 0;
        _row++;
    }
    else if (c == '\t')
    {
        _col += 4;
    }
    else
    {
        _col++;
    }
}
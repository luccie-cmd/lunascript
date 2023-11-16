// For any core defined functions (Loc)
#include "core.hh"

luna::Loc luna::makeloc(std::string file){
    luna::Loc ret;
    // Most (all) editors start counting from: File:1:1 so we do the same
    // but don't count the collumn from 1 but from 0 (doesn't know why it works it just does)
    ret.col = 0;
    ret.row = 1;
    ret.file = file;
    return ret;
}
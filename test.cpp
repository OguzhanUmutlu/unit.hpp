#include "Unit.hpp"
using namespace Unit::all;

int main() {
    auto x = 10_m;
    auto x2 = 10_dm;
    auto y = 2_s;
    auto z = 2_m;
    auto m = 10_mol;


    auto v1 = x / y;
    auto v2 = z / y;
    // auto v3 = x + y; // error
    auto v4 = x + z;

    return 0;
}

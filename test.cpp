#include "Unit.hpp"
using namespace Unit::all;

int main() {
    auto a = 2 / 10_m;
    auto b = 2_m;
    auto c = 2_s;

    auto v1 = a / c;
    auto v2 = b / c;
    // auto v3 = x + y; // error
    // auto v4 = a + b; // error

    std::cout << a * b * b * b * b * c << std::endl;

    return 0;
}

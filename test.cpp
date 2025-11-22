#include "Unit.hpp"
using namespace Unit::all;

int add(int x, int y) {
    return x + y;
}

template <auto fn>
int test(int x, int y) {
    return fn(x, y);
}

int main() {
    std::cout << test<add>(1, 2) << std::endl;
    //using t = deci<decltype(10_dm * 10_dm * 10_dm)>;
    //std::cout << 10.0_N << "\n";             // prints: 10 kg*m*s^-2

    return 0;
}

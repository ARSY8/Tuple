#include <iostream>
#include <vector>
#include "sort.hpp"

int main() {
    std::vector<int> v = { 5, 1, 4, 2, 8, 0, 3 };

    quick_sort(v.begin(), v.end(), [](int a, int b) { return a < b; });

    for (int x : v)
        std::cout << x << " ";
    std::cout << std::endl;
}
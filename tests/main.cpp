#include <iostream>

int main()
{
    unsigned x = -1;

    int y = *reinterpret_cast<int*>(&x);

    std::cout << x << std::endl;
    std::cout << y << std::endl;

    return 0;
}
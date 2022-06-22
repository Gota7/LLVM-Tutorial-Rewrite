#include <iostream>

extern "C"
{
    double testFun();
}

int main()
{
    std::cout << std::to_string(testFun()) << std::endl;
    return 0;
}
#include "iostream"
#include "include/fcomplex.h"

int main()
{
    fcomplex<double> z(1,2);
    std::cout << "abs(z) = " << __cabsf__(z) << "\n";
    std::cout << "arg(z) = " << __cargf__(z) << "\n";
    return 0;
}
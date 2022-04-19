#include "iostream"
#include "include/fcomplex.h"

int main()
{
    fcomplex<double> z(1,2);
    std::cout << "real(z) = " << __realf__(z) << "\n";
    std::cout << "imag(z) = " << __imagf__(z) << "\n";
    return 0;
}
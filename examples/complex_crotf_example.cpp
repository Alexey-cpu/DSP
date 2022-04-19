#include "iostream"
#include "include/fcomplex.h"

int main()
{
    fcomplex<double> f = __crotf__<double>(60.0f, 0);
    std::cout << "real(f) = " << __realf__(f) << "\n";
    std::cout << "imag(f) = " << __imagf__(f) << "\n";
    return 0;
}
#include "iostream"
#include "include/fcomplex.h"

int main()
{
    fcomplex<double> z(1,2);
    fcomplex<double> f = __coshf__(z);
    std::cout << "real(f) = " << __realf__(f) << "\n";
    std::cout << "imag(f) = " << __imagf__(f) << "\n";
    return 0;
}
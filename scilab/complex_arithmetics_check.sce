clc; clear;

disp(" initials: \n")
z0 = 1.0 + %i * 2.0;
z1 = 2.0 + %i * 0.5;
z2 = 3.5 + %i * 4.5;

disp(" complex functions: \n")
sqrt(z0)
z0 * z1
z0 / z1
z0 + z1 + z2
z0 - z1 - z2
z0 * z1 / z2
( z1 - z2 ) / ( z1 + z2 )
z0 = z0 + z2
z1 = z1 * z2
z2 = z2 / z1

disp(" complex functions: \n")
sinh(z0)
cosh(z0)
tanh(z0)
1/tanh(z0)
z0/abs(z0)
z0 = z0 * ( cos( 60 * %pi / 180 ) + %i * sin(  60 * %pi / 180 ) )
abs(z0)
180+atan( imag(z0) / real(z0) ) * 180 / %pi

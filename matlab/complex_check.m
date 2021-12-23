% input data:
z0 = 1.0 + i*2.0;
z1 = 2.0 + i*0.5;
z2 = 3.5 + i*4.5;
sqrt( z0 )

% arithmetics:
z0 + z1 + z2
z0 - z1 - z2
z0 * z1 / z2
(z1-z2)/(z1+z2)
z0 = z0 + z2
z1 = z1 * z2
z2 = z2 / z1;

% functions:
sqrt( z0 )
sinh( z0 )
cosh( z0 )
tanh( z0 )
1/tanh( z0 )
abs(z0)
angle(z0)*180/pi
conj(z0)
z0 / abs(z0)
z0 = z0 * ( cos( 60 * pi / 180 ) + i * sin( 60 * pi / 180 ) )

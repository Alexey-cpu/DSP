# DSP 1.0.0

## Description

**DSP** is a low level C++ digital signal processing (DSP) toolbox aimed at intellegent electronic devices (IDE) interaction.

The library is self contained and stand-alone, all you need is to simply copy the needed **.h** files into your project to get started.

## Content

The library provides basic DSP components: IIR and FIR filters, Fourier transforms etc.
Also, the library provides C++ classes to manipulate with COMTRADE files which allows read, write COMTRADE files and emulate DSP components operation
on the signals provided by your COMTRADE files. At this version the COMTRADE C++ classes work only with textual COMTRADEs only.

## Availability

Open command line, and clone repo using the following git commands:

```
git clone https://github.com/Alexey-cpu/DSP.git
```

## Licence

Your use or distribution of **DSP** or any modified version of
**DSP** implies that you agree to this License.

This library is free software; you can redistribute it and/or
modify it under the terms of the **GNU General Public
License** as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but **WITHOUT ANY WARRANTY**; without even the implied warranty of
**MERCHANTABILITY** or **FITNESS FOR A PARTICULAR PURPOSE**.  See the **GNU
General Public License** for more details.

You should have received a copy of the **GNU General Public
License** along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
USA

Permission is hereby granted to use or copy this program under the
terms of the **GNU GPL**, provided that the Copyright, this License,
and the Availability of the original version is retained on all copies.
User documentation of any code that uses this code or any modified
version of this code must cite the Copyright, this License, the
Availability note, and "Used by permission." Permission to modify
the code and to distribute modified code is granted, provided the
Copyright, this License, and the Availability note are retained,
and a notice that the code was modified is included.

## Quick start in Qt

The library is provided with pre-configured Qt projects located inside **qtprojects** folder and aimed at testing the library.
As the library is header-only you don't need to compile anything, just copy files you need into your project.

## Example

The following example function how to use Butterworth filter to manipulate with a signal and how to write COMTRADE file with appropriate signals:

```
int filters_butt_example(string _LogsDirectory)
{
    typedef float __type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // create COMTARDE registrator
    Comtrade registrator
            (
                "filters_ellip_example",
                Fn,
                Fs,
                EmulationDuration,
                {
                    new ComtradeAnalogChannel("xt"),
                    new ComtradeAnalogChannel("yt")
                }
            );

    // retrieve channels
    ComtradeAnalogChannel* xt = registrator.find_analog_channel("xt");
    ComtradeAnalogChannel* yt = registrator.find_analog_channel("yt");

    // generator initialization
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);


    // filter initialization
    butterworth<__type> filter;
    filter.init(Fs, 10, filter_type::bandpass, {100 , 400} );
    filter.show();

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick(), k++ )
        {
            __type signal = gen.sine( 1, Fn, 0, time );
            __type output = filter(&signal);

            xt->set_sample( k, signal );
            yt->set_sample( k, output );
        }
    }

    registrator.to_file( _LogsDirectory, STRINGIFY(filters_butt_example) );

    return 0;
}

int main()
{
    // setup directories
    std::string logsDirectory = "C:/Qt_projects/DSP/logs";

    // run tests / examples
    filters_butt_example(logsDirectory);

    return 0;
}

```

More examples are located in **DSP/source/dsp/tests** folder.
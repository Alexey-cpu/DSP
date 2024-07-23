# DSP 1.0.0

## Description

**DSP** is a low level C++ digital signal processing (DSP) toolbox aimed at intellegent electronic devices (IDE) interaction.

The library is self contained and stand-alone, all you need is to simply copy the needed .h files into your project to get started.

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

The following example shows how to solve sparse linear system A * x = b using SparseKernel library:

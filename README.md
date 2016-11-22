# RT-WDF
RT-WDF is a real-time capable Wave Digital Filter framework for circuit modeling, with support for arbitrary topologies and multiple/multiport non-linearities. It was introduced in a [ DAFx-16 paper ](Documentation/40-DAFx-16_paper_35-PN.pdf) and comes with a full [API reference documentation](https://rt-wdf.github.io/rt-wdf_lib/).

The repository is divided into four parts:

## Libs
This folder contains the actual code of RT-WDF. It also contains the Linear Algebra Lib 'armadillo', and the sample rate conversion framework [r8brain-free](https://github.com/avaneev/r8brain-free-src).
    
## wdfRenderer
The wdfRenderer is an offline audio renderer which processes .wav files through WDF trees and writes an output file rtwdf.wav into your home directory.
    
## Documentation
This folder contains the [DAFx-16 paper](Documentation/40-DAFx-16_paper_35-PN.pdf) as well as the [doxygen](http://doxygen.org) file to generate the [API reference documentation](https://rt-wdf.github.io/rt-wdf_lib/).

After installing [doxygen](http://doxygen.org), run `doxygen ./Documentation/Doxyfile` to generate the html documentation in a new folder at `../rt-wdf-doc`, which corresponds to the gh-pages branch.

## Toolchain
Here is the place for the [JUCE framework](https://github.com/julianstorer/JUCE).
    
# Getting started
To get started, check out the [wdfRenderer](wdfRenderer/JUCE/wdfRenderer) project, which runs the [reference circuits](Examples) from the examples folder. 

To build the wdfRenderer, make sure to install [armadillos](http://arma.sourceforge.net/) and it's dependencies (*lapack* and *blas* on Linux systems).

# Contact
Drop a line to rt-wdf@e-rm.de to get in touch!

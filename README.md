# RT-WDF

This is the RT-WDF repository. It is divided into four partitions:

### Libs:
This folder contains the actual code of RT-WDF. It also contains the Linear Algebra Lib 'armadillo', and the sample rate conversion framework [r8brain-free](https://github.com/avaneev/r8brain-free-src).
    
### wdfRenderer
The wdfRenderer is an offline audio renderer which processes .wav files through WDF trees and writes an output file rtwdf.wav into your home directory.
    
### Documentation:
This folder contains documentation about the library.

### Toolchain:
Here is the place for the [JUCE framework](https://github.com/julianstorer/JUCE).
    
# Getting started

To get started, check out the [wdfRenderer](https://github.com/m-rest/rt-wdf/tree/master/wdfRenderer/JUCE/wdfRenderer) project, which operates the [reference circuits](https://github.com/m-rest/rt-wdf/tree/master/Examples) from the examples folder. 

Make sure to install [armadillos](http://arma.sourceforge.net/) dependencies according to their instructions to build the wdfRenderer software, namely *lapack* and *blas* on Linux systems.


# Contact

Drop a line to rt-wdf@e-rm.de to get in touch with any questions.

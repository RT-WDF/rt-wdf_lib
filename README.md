# RT-WDF

RT-WDF is a Real-Time capable Wave Digital Filter Framework for Circuit Modeling with support for arbitrary topologies and multiple/multiport Nonlinearities. It was introduced in a paper at DAFx-16 which can be found [here](Documentation/40-DAFx-16_paper_35-PN.pdf).

The repository is divided into four partitions:

### Libs:
This folder contains the actual code of RT-WDF. It also contains the Linear Algebra Lib 'armadillo', and the sample rate conversion framework [r8brain-free](https://github.com/avaneev/r8brain-free-src).
    
### wdfRenderer
The wdfRenderer is an offline audio renderer which processes .wav files through WDF trees and writes an output file rtwdf.wav into your home directory.
    
### Documentation:
This folder contains the [API reference documentation](https://cdn.rawgit.com/m-rest/rt-wdf/master/Documentation/doc/html/index.html) of the RT-WDF library as well as the DAFx-16 paper which describes it's initial architecture.

You will need [doxygen](http://doxygen.org) to regenerate the documentation. Running `doxygen ./Documentation/Doxyfile` will generate the html documentation in a new folder at `../rt-wdf-doc`, which corresponds to the gh-pages branch.

### Toolchain:
Here is the place for the [JUCE framework](https://github.com/julianstorer/JUCE).
    
# Getting started

To get started, check out the [wdfRenderer](wdfRenderer/JUCE/wdfRenderer) project, which operates the [reference circuits](Examples) from the examples folder. 

Make sure to install [armadillos](http://arma.sourceforge.net/) dependencies according to their instructions to build the wdfRenderer software, namely *lapack* and *blas* on Linux systems.


# Contact

Drop a line to rt-wdf@e-rm.de to get in touch with any questions.

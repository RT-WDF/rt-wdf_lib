# RT-WDF
RT-WDF is a real-time capable Wave Digital Filter library for circuit modeling, with support for arbitrary topologies and multiple/multiport non-linearities. It was introduced in a [ DAFx-16 paper ](Documentation/40-DAFx-16_paper_35-PN.pdf) and comes with a full [API reference documentation](https://rt-wdf.github.io/rt-wdf_lib/).

The repository is divided into two parts:

## Libs
This folder contains the actual code of RT-WDF.
    
## Documentation
This folder contains the [DAFx-16 paper](Documentation/40-DAFx-16_paper_35-PN.pdf) as well as the [doxygen](http://doxygen.org) file to generate the [API reference documentation](https://rt-wdf.github.io/rt-wdf_lib/).

After installing [doxygen](http://doxygen.org), run `doxygen ./Documentation/Doxyfile` to generate the html documentation in a new folder at `../rt-wdf-doc`, which corresponds to the gh-pages branch.
    
# Dependencies
RT-WDF depends on [armadillo](http://arma.sourceforge.net/). Make sure to install the library and it's dependencies. 

# Getting started
To get started, check out the [wdfRenderer](http://github.com/RT-WDF/rt-wdf_renderer) project, which runs some [reference circuits](https://github.com/RT-WDF/rt-wdf_renderer/tree/master/Circuits). 

# Contact
Drop a line to rt-wdf@e-rm.de to get in touch!

# Acknowledgements
Initial development of the RT-WDF libary was partly sponsored by [CCRMA](https://ccrma.stanford.edu)/Stanford University. A special thanks goes to Julius Orion Smith III for making this possible.

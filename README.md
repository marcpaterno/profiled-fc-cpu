# CPU-based profiled Feldman-Cousins support code

This repository contains C++ code (CPU-only code) to support the profiled Feldman-Cousins statistical analysis technique.

## To get the required libraries and compilers, I recommend using Conda

The file `environment.yml` contains the description of a Conda environment that will contain all the software necessary for building this project.
To create the Conda environment, you must first install Conda (I recommend using the Mamba variant).
The command to build the environment is:

    # Replace 'conda' with 'mamba', if you installed mamba.
    conda env create --name for-fc-cpu --file=environment.yml

Note that when you activate this Conda environment, the environment variable `CXX` will be defined to be the name of the C++ compiler in the environment.
The CMake build instructions below make use of this fact.
If you choose to get the compiler and Minuit2 some other way, it is up to you to tell CMake what compiler to use, and for you to make sure that the same compiler was used to build Minuit2.

## Use CMake to generate the build system

I use CMake to direct the building of the software.
The build also requires the Catch2 testing system.
The CMake build will handle this automatically; if you choose not to use CMake, you need to make sure get and build Catch2.
You will also need to direct the build to find the Minuit2 headers and libraries, and the Catch2 headers and libraries.

If you are using CMake, only out-of-source builds are supported.

    # create the build directory, and move into it
    mkdir -p release-build
    cd release-build
    
    # create the build system, using Ninja
    cmake -G Ninja ../
    
To build the software, run `ninja` in the build directory.
To run the tests, run `ctest` in the build directory.

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

If you choose not to use a conda environment, then you must make sure that the blis, fmt and tbb libraries and headers are available to your compiler. On macOS, if you install them with Homebrew, everything will work.

## Use CMake to generate the build system

I use CMake to direct the building of the software.
The build also requires the Catch2 testing system and the Dlib library.
The CMake build will handle obtaining both automatically; if you choose not to use CMake, you need to make sure get and build Catch2.
You will also need to direct the build to find the Catch2 and Dlib headers and libraries.

If you are using CMake, only out-of-source builds are supported.

    # create the build directory, and move into it
    mkdir -p release-build
    cd release-build

    # Note: in the following, most of the flags passed to cmake are to control
    # the building of 'dlib', which is downloaded and built automatically. We
    # turn off the graphics-related and GPU-related parts of the package.
    
    # create the build system, in a release build using the clang++ and clang
    # compilers found on $PATH. If you are using a conda environment built as
    # described above, the environment variable $CXX will be defined and used
    # automatically by cmake to find the compiler. Otherwise, CMake will use
    # its own mechanism for finding what it considers to be the right compiler
    # for you.

    # This is appropriate when, e.g., you are using the conda environment to
    # obtain some of the pre-built dependencies. This is also appropriate when
    # using a Homebrew-based build.
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=on -DDLIB_JPEG_SUPPORT=off -DDLIB_LINK_WITH_SQLITE3=off -DDLIB_USE_CUDA=off -DDLIB_PNG_SUPPORT=off -DDLIB_GIF_SUPPORT=off -DDLIB_WEBP_SUPPORT=off -DDLIB_USE_MKL_FFT=off -DDLIB_USE_FFMPEG=off -DDLIB_NO_GUI_SUPPORT=on  ..

    # Another alternative: generate an Xcode project.
    cmake -G Xcode -DBUILD_SHARED_LIBS=on -DDLIB_JPEG_SUPPORT=off -DDLIB_LINK_WITH_SQLITE3=off -DDLIB_USE_CUDA=off -DDLIB_PNG_SUPPORT=off -DDLIB_GIF_SUPPORT=off -DDLIB_WEBP_SUPPORT=off -DDLIB_USE_MKL_FFT=off -DDLIB_USE_FFMPEG=off -DDLIB_NO_GUI_SUPPORT=on  ..

    
To build the software, run `ninja` in the build directory.
To run the tests, run `ctest` in the build directory.

## The example programs

### dlib_parallel_rastrigin_example

This program demonstrates the use of a simple parallelization over otherwise serial local minimization.
This program also demonstrates the use of a task-based parallel programming model.

### dlib_parallel_rosenbrock_example

This program demonstrate the use of a simple parallelization of otherwise serial local minimization.
It uses the Rosenbrock function in *n* dimensions; it runs in about 45 seconds in 1000 dimensions on a 12-core M2 MacBook pro.

### dlib_parallel_rastrigin_example_5d

This is similar to `dlib_parallel_rastrigin_example` except that it uses an argument type that has its size fixed at compile time, rather than at run time.
This allows measurement of the performance cost of using the more flexible run time size.

### dlib_serial_rosenbrock_example

This does a local minimization of the Rosenbrock function in arbitrary dimensions, starting from a point specified by the user.
It counts both the number of steps and the number of calls to the Rosenbrock function made by the minimizer.


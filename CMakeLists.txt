cmake_minimum_required(VERSION 3.26)
project(
  profiled_fc_cpu
  VERSION 0.3.0
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)
include(CTest)

# We expect to find the following dependencies in the environment.
# This might be the Conda environment, if we are using one, or it might
# be the base environment of the platform. The cmake command line can
# be passed variables to help find the right installations; see the
# CMake documentation for each package to find out what variables are
# used by each.
find_package(TBB 2021.10.0 REQUIRED)
find_package(fmt REQUIRED)

# We use catch2 for testing.
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0 # or a later release
  GIT_SHALLOW TRUE)

# We are using a fork of the dlib repository so that we can add
# features if we so desire.
# We are using the head of master in an experiment to see if this
# provides good integration for simultaneous development of two
# libraries (profiled_fc_cpu and dlib).
FetchContent_Declare(
  dlib
  GIT_REPOSITORY https://github.com/marcpaterno/dlib.git
  GIT_TAG profiled_fc_features
  GIT_SHALLOW false)

FetchContent_MakeAvailable(catch)
FetchContent_MakeAvailable(dlib)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.26)
project(
  profiled-fc-cpu
  VERSION 0.1.0
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)
include(CTest)

# We use catch2 for testing.
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0 # or a later release
  GIT_SHALLOW TRUE)

FetchContent_Declare(
  dlib
  GIT_REPOSITORY https://github.com/marcpaterno/dlib.git
  GIT_TAG 22f66ecba6fff616bc501bbfcd73334ad55f8fc3
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(catch)
FetchContent_MakeAvailable(dlib)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

find_package(Minuit2 REQUIRED)
find_package(Boost 1.82 REQUIRED COMPONENTS stacktrace_basic)
find_package(TBB 2021.10.0 REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(src)

#include "geometry.hh"
#include "minimizers.hh"
#include "protected_engine.hh"
#include "rosenbrock.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <span>
#include <string>
#include <utility>

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rosenbrock_dlib_wrapper(pfc::column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  std::span xx = x;
  return pfc::vec_rosenbrock(xx);
}

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Please specify the number of dimensions to use\n";
    return 1;
  }
  long const ndim = std::stol(argv[1]);

  int const num_starting_points = oneapi::tbb::info::default_concurrency();

  // Create shared state for answer.
  // We're done when we have found a minimum with a value < 1.0e-6.
  auto [solutions, num_attempts] = pfc::find_global_minimum(
    rosenbrock_dlib_wrapper, ndim, num_starting_points, 1.0e-6);
  if (solutions.empty()) {
    std::cerr << "No solutions were found!\n";
    return 1;
  }

  // We print this count information to standard error so that redirecting
  // standard output to a file does not result in this text also being
  // redirected.
  std::cerr << "A total of " << num_attempts << " minimizations were done.\n";
  pfc::print_report(solutions, std::cout);
}

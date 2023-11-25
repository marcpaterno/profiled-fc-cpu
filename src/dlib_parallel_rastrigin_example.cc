#include "geometry.hh"
#include "minimizers.hh"
#include "rastrigin.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <mutex>
#include <span>
#include <string>
#include <utility>

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rastrigin_dlib_wrapper(pfc::column_vector const& x)
{
  std::span xx = x;
  return pfc::rastrigin(xx);
}

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Please specify the number of dimensions to use\n";
    return 1;
  }
  long const ndim = std::stol(argv[1]);

  // We will start up as many tasks as TBB says we have threads.
  int const num_starting_points = oneapi::tbb::info::default_concurrency();

  // Create shared state for answer.
  // We're done when we have found a minimum with a value < 1.0e-6.
  auto start = pfc::now_in_milliseconds();
  auto [solutions, num_attempts] = pfc::find_global_minimum(
    rastrigin_dlib_wrapper, ndim, num_starting_points, 1.e-6);
  auto stop = pfc::now_in_milliseconds();

  auto running_time = stop - start;

  if (solutions.empty()) {
    std::cerr << "No solutions were found!\n";
    return 1;
  }

  // We print this count information to standard error so that redirecting
  // standard output to a file does not result in this text also being
  // redirected.
  std::cerr << "A total of " << num_attempts << " minimizations were done in "
            << running_time << " milliseconds.\n"
            << num_attempts / running_time << " solutions per millisecond\n.";
  pfc::print_report(solutions, std::cout);
}

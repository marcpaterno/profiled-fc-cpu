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
template <int N>
double
rastrigin_dlib_wrapper(pfc::fixed_vector<N> const& x)
{
  std::span xx = x;
  return pfc::rastrigin(xx);
}

// This is the function that does all the minimization work. We use this to
// provide a scope that ensures that the object 'solutions' has a longer
// lifetime than the task_group 'tasks'.
// It is a blocking function that schedules parallel work, and waits until that
// work is done before returning.
// The template parameter N determines the dimensionality of the rastrigin
// function we will use.

int
main()
{
  // We will start up as many tasks as TBB says we have threads.
  int const num_starting_points = oneapi::tbb::info::default_concurrency();

  auto start = pfc::now_in_milliseconds();
  auto [solutions, num_attempts] = pfc::find_global_minimum_fixed(
    rastrigin_dlib_wrapper<5>, num_starting_points, 1.0e-6);
  auto stop = pfc::now_in_milliseconds();
  auto running_time = stop - start;

  if (solutions.empty()) {
    std::cerr << "No solutions were found in " << running_time
              << " milliseconds\n";
    return 1;
  }

  // We print this count information to standard error so that redirecting
  // standard output to a file does not result in this text also being
  // redirected.
  std::cerr << "A total of " << num_attempts
            << " minimizations were done in " << running_time
            << " milliseconds.\n"
            << num_attempts / running_time << " solutions per millisecond\n.";

  pfc::print_report(solutions, std::cout);
}

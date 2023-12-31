#include "geometry.hh"
#include "helical_valley.hh"
#include "minimizers.hh"
#include "protected_engine.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <limits>
#include <span>
#include <string>
#include <utility>

int
main()
{
  long const ndim = 3;
  int const num_starting_points = oneapi::tbb::info::default_concurrency();
  std::cerr << "We are using: " << num_starting_points << " starting points\n";

  // We will be done when we find a minimum function value that is less than
  // tolerance.
  double const tolerance = 1.0e-6;

  // We will search withing this starting_volume.
  auto starting_volume = pfc::make_box_in_n_dim(ndim, -1.0e6, 1.0e6);

  pfc::CountedHelicalValley helical_valley;

  auto [solutions, num_attempts] = pfc::find_global_minimum(
    helical_valley, ndim, starting_volume, num_starting_points, tolerance);
  if (solutions.empty()) {
    std::cerr << "No solutions were found!\n";
    return 1;
  }

  // We print this count information to standard error so that redirecting
  // standard output to a file does not result in this text also being
  // redirected.
  std::cerr << " A total of " << num_attempts << " minimizations were done.\n";
  std::sort(solutions.begin(), solutions.end());
  print_report(solutions, std::cout);
}

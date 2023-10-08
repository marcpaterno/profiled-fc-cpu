#include "geometry.hh"
#include "rastrigin.hh"

#include "dlib/global_optimization.h"

#include <iostream>
#include <stdexcept>
#include <utility>

using namespace pfc;

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rastrigin_dlib_wrapper(column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::rastrigin({begin(x), end(x)});
}

bool
do_one_minimization(column_vector const& lower_bounds,
                    column_vector const& upper_bounds,
                    int dim,
                    long maxcalls,
                    std::ostream& os)
{
  static bool first = true;
  auto start = std::chrono::steady_clock::now();
  dlib::function_evaluation result = dlib::find_min_global(
    rastrigin_dlib_wrapper,
    lower_bounds,
    upper_bounds,
    dlib::max_function_calls(maxcalls) // max function evaluations
  );
  auto stop = std::chrono::steady_clock::now();
  auto delta_t = stop - start;
  // Print out the header the first time we're called
  if (first) {
    first = false;
    os << "maxcalls\tfmin";
    for (int i = 0; i != dim; ++i) {
      os << "\tx" << i;
    }
    os << "\ttime\n";
  }
  os << maxcalls << '\t' << result.y;
  for (int i = 0; i != dim; ++i) {
    os << '\t' << result.x(i);
  }
  os << '\t'
     << std::chrono::duration_cast<std::chrono::microseconds>(delta_t).count()
     << '\n';
  return abs(result.y) < 1.0e-6;
}

int
main()
{
  // Create task group
  // Create shared state for answer
  int num_starting_points = 5;
  for (int i = 0; i != num_starting_points; ++i) {
    // schedule a task that:
    //    1. starts at a new random point
    //    2. runs the local minimizer
    //    3. puts the result into the shared state
    //    4. looks at the shared state to see if we're done yet
    //    5. if we're not done yet, schedules a new task starting from a new
    //    random point
  }
  // Wait for all the tasks in the group to finish.
  // When they have we are done.

  // Print out the result
}

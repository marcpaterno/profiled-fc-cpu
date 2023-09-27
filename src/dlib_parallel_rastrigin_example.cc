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
  // Set up full region for test.
  region full_region({-19.0, -18.0, -17.0}, {1.0, 2.0, 3.0});
  // Do 7 levels of splitting to each "set" of regions; this generates 2**7 =
  // 128 regions.
  std::vector<region> regions =
    make_splits(7, std::vector<region>{full_region});
}

#include "geometry.hh"
#include "rastrigin.hh"

#include "dlib/global_optimization.h"

#include <iostream>

pfc::bounds
pfc::make_bounds(int dim)
{
  // We intentionally make the bounds non-symmetric, to avoid having the
  // optimizer take advantage of the symmetry of the rastrigin function.
  bounds result(dim);
  for (int i = 0; i < dim; ++i) {
    result.lower(i) = -4.0 + i;
    result.upper(i) = 1.0 + i;
  }
  return result;
}

// We have written the rastrigin function to be callable with a vector of
// arbitrary length, so we use the dynamic-sized version of column vector.
using column_vector = dlib::matrix<double, 0, 1>;

double
rastrigin_dlib_wrapper(column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::rastrigin({begin(x), end(x)});
}

// Structure to supply the boundaries for the minimization search region.
struct bounds {
  explicit bounds(int dim) : lower(dim), upper(dim) {}
  column_vector lower;
  column_vector upper;
};

bounds
make_bounds(int dim)
{
  // We intentionally make the bounds non-symmetric, to avoid having the
  // optimizer take advantage of the symmetry of the rastrigin function.
  bounds result(dim);
  for (int i = 0; i < dim; ++i) {
    result.lower(i) = -4.0 + i;
    result.upper(i) = 1.0 + i;
  }
  return result;
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
main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Please supply the dimensionality to be used\n";
    return 1;
  }
  std::vector<std::string> args(argv + 1, argv + argc);
  int const dim = std::stoi(args[0]);
  if ((dim <= 0) or (dim > 20)) {
    std::cerr << "Please supply a positive dimensionality less than 20\n";
    return 2;
  }
  auto [lower_bounds, upper_bounds] = make_bounds(dim);

  std::cout.precision(17);
  for (long i = 0; i != 24; ++i) {
    long maxcalls = std::pow(2, i);
    bool converged =
      do_one_minimization(lower_bounds, upper_bounds, dim, maxcalls, std::cout);
    if (converged)
      break;
  }
}

#include "rastrigin.hh"

#include "dlib/global_optimization.h"

#include <iostream>

// We have written the rastrigin function to be callable with a vector of
// arbitrary length, so we use the dynamic-sized version of column vector.
using column_vector = dlib::matrix<double, 0, 1>;

double
rastrigin_dlib_wrapper(column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::rastrigin({begin(x), end(x)});
}

bool
do_one_minimization(dlib::thread_pool& threads, long maxcalls, std::ostream& os)
{
  static bool first = true;
  dlib::function_evaluation result = dlib::find_min_global(
    threads,
    rastrigin_dlib_wrapper,
    {-10.0, -7.0, -13.0},              // lower bounds
    {5.0, 8.0, 2.0},                   // upper bounds
    dlib::max_function_calls(maxcalls) // max function evaluations
  );

  // Print out the header the first time we're called
  if (first) {
    os << "maxcalls\tfmin\tx0\tx1\tx2\n";
    first = false;
  }
  os << maxcalls << '\t' << result.y;
  for (int i = 0; i < 3; ++i) {
    os << '\t' << result.x(i);
  }
  os << '\n';
  return abs(result.y) < 1.0e-6;
}

int
main()
{
  std::cout.precision(17);
  dlib::thread_pool threads(8); // set this based on your hardware
  for (long i = 0; i < 20; ++i) {
    long maxcalls = std::pow(2, i);
    bool converged = do_one_minimization(threads, maxcalls, std::cout);
    if (converged)
      break;
  }
}
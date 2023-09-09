#include "rosenbrock.hh"

#include "dlib/global_optimization.h"
#include "dlib/optimization.h"

#include <iostream>

// ----------------------------------------------------------------------------

// dlib::matrix<type, nr, nc> is a matrix with nr rows and nc columns.
// nr and nc are both of type long.
// Negative values for nc and nr are not allowed.
// A value of 0 means that that dimension has variable size; a value greater
// than zero means the dimension has fixed size. Choosing a fixed-size value
// may allow for more efficient code generation (e.g., allowing loop unrolling).

using column_vector = dlib::matrix<double, 0, 1>;

using namespace dlib;
using namespace std;

double
rosenbrock_dlib_wrapper(const column_vector& m)
{
  return pfc::rosenbrock(m(0), m(1));
}

int
main()
{
  std::cout.precision(17);
  column_vector const starting_point = {-1.2, 1.0};

  column_vector answer = starting_point;
  dlib::find_min_using_approximate_derivatives(
    dlib::bfgs_search_strategy(),
    objective_delta_stop_strategy(1e-7),
    rosenbrock_dlib_wrapper,
    answer,
    -1);
  std::cout << "using bfgs:\n"
            << answer << "minimal value is: " << rosenbrock_dlib_wrapper(answer)
            << '\n';

  answer = starting_point;
  dlib::find_min_using_approximate_derivatives(
    dlib::lbfgs_search_strategy(10),
    dlib::objective_delta_stop_strategy(1e-7),
    rosenbrock_dlib_wrapper,
    answer,
    -1);
  std::cout << "\nusing lbfgs:\n"
            << answer << "minimal value is: " << rosenbrock_dlib_wrapper(answer)
            << '\n';

  // Note that dlib::find_min_global does *not* require a function that takes
  // a column_vector argument; it can just take the appropriate number of
  // arguments of type double! Unfortunately, it is not clever enough to
  // give a compilation failure if the specification of the lower bounds and
  // the upper bounds have the wrong number of values.
  dlib::function_evaluation result =
    dlib::find_min_global(pfc::rosenbrock,
                          {-10, -10},                    // lower bounds
                          {10, 10},                      // upper bounds
                          std::chrono::milliseconds(500) // run this long
    );

  std::cout << "\nusing find_min_global:\n"
            << result.x << "minimum value is: " << result.y << '\n';
}

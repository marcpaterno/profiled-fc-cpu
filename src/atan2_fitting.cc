#include "minimizers.hh"

#include <cmath>
#include <iostream>

using column_vector = pfc::column_vector;

// This is the function we are looking to fit.
inline double
better_atan_core(double z, column_vector const& a)
{
  double t1= a(0) * z;
  double t2 = z*(z-1)*(a(1) + a(2)*z*(a(3) + z));
  return t1 - t2;
}

// Determine the maximum absolute deviation of better_atan_core, with the given
// set of parameters, from the std::atan2 function, over the range of y [0,1], with x=1.0
inline double
max_abs_deviation(column_vector const& params)
{
  double max_abs_dev = -1.0;
  int const NPOINTS = 1000;
  for (int i = 0; i != NPOINTS + 1; ++i) {
    double const x = static_cast<double>(i) / NPOINTS;
    double const fx = better_atan_core(x, params);
    double const delta_f = std::abs(fx - std::atan2(x, 1.0));
    if (delta_f > max_abs_dev) {
      max_abs_dev = delta_f;
    }
  }
  return max_abs_dev;
}

// The objective function we will minimize.
inline double
objective_function(column_vector const& params)
{
  return max_abs_deviation(params);
}

int
main()
{
  long const ndim = 4;
  double const tolerance = 1.e-6;
  long const num_starting_points = 20;
  long max_attempts = 1 * 1000;
  auto starting_volume = pfc::make_box_in_n_dim(ndim, -1.0, 1.0);
  auto [solutions, num_attempts] = pfc::find_global_minimum(objective_function,
                                                            ndim,
                                                            starting_volume,
                                                            num_starting_points,
                                                            tolerance,
                                                            max_attempts);
  std::cout << num_attempts << " fit attempts were done. Max allowed was "
            << max_attempts << '\n';
  pfc::print_report(solutions, std::cout);
}

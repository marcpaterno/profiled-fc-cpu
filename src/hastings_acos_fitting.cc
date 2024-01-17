#include "minimizers.hh"

#include <cmath>
#include <iostream>

using column_vector = pfc::column_vector;

// This is the function we are looking to fit.
inline double
better_hastings(double x, column_vector const& params)
{
  double ret = params(0);
  for (long i = 1; i != params.size(); ++i) {
    ret *= x;
    ret += params(i);
  }
  return ret * std::sqrt(1.0 - x);
}

// Determine the maximum absolute deviation of better_hastings, with the given
// set of parameters, from the std::acos function, over the range of x [0,1].
inline double
max_abs_deviation(column_vector const& params)
{
  double max_abs_dev = -1.0;
  int const NPOINTS = 1000;
  for (int i = 0; i != (1 * NPOINTS) + 1; ++i) {
    double const x = static_cast<double>(i) / NPOINTS;
    double const fx = better_hastings(x, params);
    double const delta_f = std::abs(fx - std::acos(x));
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
main(int argc, char** argv)
{
  if (argc != 3) {
    std::cerr << "Please specify the number of fit parameters and the minimal "
                 "tolerance to achieve\n.";
    return 1;
  }

  long const ndim = std::atol(argv[1]);
  double const tolerance = std::atof(argv[2]);
  long const num_starting_points = 12;
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
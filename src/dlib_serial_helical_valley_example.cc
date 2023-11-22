#include "dlib/optimization.h"
#include "geometry.hh"
#include "helical_valley.hh"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

inline double
now_in_milliseconds()
{
  using namespace std::chrono;
  auto const t = steady_clock::now().time_since_epoch();
  return duration<double>(t).count() * 1000.0;
}

pfc::column_vector
make_starting_point(int argc, char** argv)
{
  pfc::column_vector starting_point(argc - 1);
  for (int i = 1; i != argc; ++i) {
    // We convert to a string so we can use stod's automatic error detection.
    std::string s(argv[i]);
    starting_point(i - 1) = std::stod(s);
  }
  return starting_point;
}

int
main(int argc, char** argv)
{
  if (argc != 4) {
    std::cerr << "Please specify three starting coordinates.\n";
    return 1;
  }

  pfc::CountedHelicalValley func;
  auto starting_point = make_starting_point(argc, argv);
  auto ndim = starting_point.size();
  auto location = starting_point;
  auto tstart = now_in_milliseconds();

  auto [f_value, nsteps] = dlib::find_min_using_approximate_derivatives(
    dlib::bfgs_search_strategy(),
    dlib::objective_delta_stop_strategy(1.0e-6),
    func,
    location,
    -1.0);
  auto tstop = now_in_milliseconds();

  std::cout << f_value << '\t';
  std::cout << location(0);
  for (int i = 1; i != ndim; ++i) {
    std::cout << '\t' << location(i);
  }
  std::cout << '\t' << nsteps << '\t' << func.ncalls() << '\n';
}

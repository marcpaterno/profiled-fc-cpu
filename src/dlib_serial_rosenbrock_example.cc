#include "dlib/optimization.h"
#include "geometry.hh"
#include "rosenbrock.hh"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

// This callable class wraps the rosenbrock function to allow calling from dlib
// and also counts the numbers of times operator() is invoked.
class CountedRosenbrock {
public:
  double
  operator()(pfc::column_vector const& x) const
  {
    _ncalls += 1;
    using namespace std;
    std::span xx = x;
    return pfc::vec_rosenbrock(xx);
  }
  long
  ncalls() const
  {
    return _ncalls;
  }

private:
  mutable long _ncalls = 0;
};

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
  if (argc < 2) {
    std::cerr << "Please specify the starting coordinates.\n";
    return 1;
  }

  CountedRosenbrock func;
  auto starting_point = make_starting_point(argc, argv);
  auto ndim = starting_point.size();
  auto location = starting_point;
  auto tstart = now_in_milliseconds();

  auto [f_value, nsteps, steps] = dlib::find_min_using_approximate_derivatives(
    dlib::bfgs_search_strategy(),
    dlib::objective_delta_stop_strategy(1.0e-6),
    func,
    location,
    -1.0);
  auto tstop = now_in_milliseconds();

  std::cout << location(0);
  for (int i = 1; i != ndim; ++i) {
    std::cout << '\t' << location(i);
  }
  std::cout << '\t' << nsteps << '\t' << func.ncalls() << '\n';
}

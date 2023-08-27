#define _GNU_SOURCE
#include "rosenbrock.hh"
#include "boost/stacktrace.hpp"
#include <iostream>

namespace pfc {

  inline double
  square(double x)
  {
    return x * x;
  }

  double
  rosenbrock(double x, double y)
  {
    return square(1.0 - x) + 100.0 * square(y - square(x));
  }

  double
  rosenbrock_traced(double x, double y)
  {
    std::cout << boost::stacktrace::stacktrace();
    return rosenbrock(x, y);
  }
}



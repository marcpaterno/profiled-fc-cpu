#ifndef PROFILED_FC_CPU_ROSENBROCK_HH
#define PROFILED_FC_CPU_ROSENBROCK_HH

#include <span>

namespace pfc {
  // rosenbrock is the standard Rosenbrock valley function.
  double rosenbrock(double x, double y);

  double vec_rosenbrock(std::span<double const> x);
}

#endif

#ifndef PROFILED_FC_CPU_ROSENBROCK_HH
#define PROFILED_FC_CPU_ROSENBROCK_HH

#include <vector>

namespace pfc {
  // rosenbrock is the standard Rosenbrock valley function.
  double rosenbrock(double x, double y);

  double vec_rosenbrock(std::vector<double> const& x);
}

#endif

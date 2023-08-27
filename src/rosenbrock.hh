#ifndef PROFILED_FC_CPU_ROSENBROCK_HH
#define PROFILED_FC_CPU_ROSENBROCK_HH

namespace pfc {
  // rosenbrock is the standard Rosenbrock valley function.
  double rosenbrock(double x, double y);

  // rosenbrock_traced uses the Boost stacktrace library to
  // print a full stack trace on each call. This is quite
  // slow, and should only be used to explore the behavior
  // of the code calling the function.
  double rosenbrock_traced(double x, double y);
}

#endif

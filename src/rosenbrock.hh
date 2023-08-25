#ifndef PROFILED_FC_CPU_ROSENBROCK_HH
#define PROFILED_FC_CPU_ROSENBROCK_HH

namespace pfc {
  inline double
  square(double x)
  {
    return x * x;
  }

  inline double
  rosenbrock(double x, double y)
  {
    return square(1.0 - x) + 100.0 * square(y - square(x));
  }
}

#endif

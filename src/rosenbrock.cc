#include "rosenbrock.hh"

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
  vec_rosenbrock(std::span<double const> x)
  {
    double sum = 0.0;
    std::size_t N = x.size();
    for (std::size_t i = 1; i != N; ++i) {
      double const t1 = x[i] - x[i - 1] * x[i - 1];
      sum += 100.0 * t1 * t1;
      double const t2 = 1.0 - x[i - 1];
      sum += t2 * t2;
    }
    return sum;
  }
}

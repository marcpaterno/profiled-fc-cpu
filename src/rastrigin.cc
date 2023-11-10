#include "rastrigin.hh"
#include "math.h" // C++20 has <numbers>, and std::numbers::pi

namespace pfc {

  double
  rastrigin(std::span<double const> x)
  {
    double sum = 10.0 * x.size();
    for (auto val : x)
      sum += val * val - 10 * cos(2. * M_PI * val);
    return sum;
  }
}

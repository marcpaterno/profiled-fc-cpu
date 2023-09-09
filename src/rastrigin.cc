#include "rastrigin_minuit_wrapper.hh"
#include "rastrigin.hh"
#include "math.h" // C++20 has <numbers>, and std::numbers::pi

namespace pfc {

  double
  rastrigin(std::vector<double> const& x)
  {
    double sum = 10.0 * x.size();
    for (auto val : x)
      sum += val * val - 10 * cos(2. * M_PI * val);
    return sum;
  }

  double
  RastriginWrapper::operator()(std::vector<double> const& x) const
  {
    return rastrigin(x);
  }

  double
  RastriginWrapper::Up() const
  {
    return 1.0;
  }
}

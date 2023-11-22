#pragma once

#include "geometry.hh"

#include <cmath>
#include <numbers>

namespace pfc {
  inline double
  theta(double x, double y)
  {
    double v = std::atan2(y, x);
    if (x < 0)
      v += std::numbers::pi;
    return 0.5 * std::numbers::inv_pi * v;
  }

  inline double
  helical_valley(pfc::column_vector const& arg)
  {
    double const x = arg(0);
    double const y = arg(1);
    double const z = arg(2);
    double const t2 = z - 10.0 * theta(x, y);
    double const t3 = std::hypot(x, y) - 1.0;
    double const t1 = t2 * t2 + t3 * t3;
    return 100.0 * t1 + z * z;
  }

  // This callable class wraps the helical valley function to
  // count the number of times operator() is invoked.
  class CountedHelicalValley {
  public:
    double
    operator()(pfc::column_vector const& x) const
    {
      _ncalls += 1;
      return pfc::helical_valley(x);
    }
    long
    ncalls() const
    {
      return _ncalls;
    }

  private:
    mutable long _ncalls = 0;
  };

}

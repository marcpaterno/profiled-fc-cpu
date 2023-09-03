#ifndef PROFILE_FC_CPU_RASTRIGIN_HH
#define PROFILE_FC_CPU_RASTRIGIN_HH

#include <vector>

#include "Minuit2/FCNBase.h"

namespace pfc {

  // rastrigin is the standard Rastring function, in as many dimenions as
  // the length of the vector 'x'.
  double rastrigin(std::vector<double> const& x);

  struct RastriginWrapper : ROOT::Minuit2::FCNBase {
    double operator()(std::vector<double> const& x) const override;
    double Up() const override;
  };
}

#endif

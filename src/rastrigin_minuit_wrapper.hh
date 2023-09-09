#ifndef PROFILE_FC_CPU_RASTRIGIN_MINUIT_WRAPPER_HH
#define PROFILE_FC_CPU_RASTRIGIN_MINUIT_WRAPPER_HH

#include <vector>
#include "Minuit2/FCNBase.h"

namespace pfc {

  struct RastriginWrapper : ROOT::Minuit2::FCNBase {
    double operator()(std::vector<double> const& x) const override;
    double Up() const override;
  };
}

#endif

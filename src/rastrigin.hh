#ifndef PROFILE_FC_CPU_RASTRIGIN_HH
#define PROFILE_FC_CPU_RASTRIGIN_HH

#include <span>

namespace pfc {

  // rastrigin is the standard Rastring function, in as many dimenions as
  // the length of 'x'.
  double rastrigin(std::span<double const> x);
}

#endif

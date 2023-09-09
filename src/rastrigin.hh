#ifndef PROFILE_FC_CPU_RASTRIGIN_HH
#define PROFILE_FC_CPU_RASTRIGIN_HH

#include <vector>

namespace pfc {

  // rastrigin is the standard Rastring function, in as many dimenions as
  // the length of the vector 'x'.
  double rastrigin(std::vector<double> const& x);
}

#endif

#ifndef PROFILED_FC_CPU_ROSENBROCK_MINUIT_WRAPPER_HH
#define PROFILED_FC_CPU_ROSENBROCK_MINUIT_WRAPPER_HH

#include "rosenbrock.hh"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Minuit2/FCNBase.h"

namespace pfc {

  // Minuit2 requires that the "function" it minimizes be implemented as a
  // callable class type that inherits from ROOT::Minuit2::FCNBase.
  //
  struct RosenbrockWrapper : ROOT::Minuit2::FCNBase {
    // Evaluate the function at the location 'x'
    double operator()(std::vector<double> const& x) const override;

    // The value returned by 'Up' is used to determine the errors in the fitted
    // parameters. This is a pure virtual function in the base calss, and so we
    // must implement it. However, we do not care about the errors reported by
    // the minimizer; they are in no way relevant for our use of a minimizer.
    // This also means that the errors reported in a minimization result are
    // meaningless.
    double Up() const override;
  };

  class TracedRosenbrockWrapper : public ROOT::Minuit2::FCNBase {
  public:
    // On each call, we capture a call stack, and update a count of unique calls
    // stacks seen thus far.
    // Writing the TracedRosenbrockWrapper to an output stream will write out
    // the unique call stacks seen, and how many times each was seen.
    double operator()(std::vector<double> const& x) const override;
    double Up() const override;

    friend std::ostream& operator<<(std::ostream& os,
                                    TracedRosenbrockWrapper const& w);

  private:
    // The collection of stacks recorded has to be mutable, so that we can
    // update it from the function call operator, which is const.
    mutable std::unordered_map<std::string, int> stacks_;
  };

}

#endif

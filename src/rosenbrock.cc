#define _GNU_SOURCE
#include "rosenbrock.hh"
#include "boost/stacktrace.hpp"
#include "rosenbrock_minuit_wrapper.hh"

#include <iostream>
#include <ostream>

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

  // Implementation of RosenbrockWrapper.
  double
  RosenbrockWrapper::operator()(std::vector<double> const& x) const
  {
    return pfc::rosenbrock(x[0], x[1]);
  }

  double
  RosenbrockWrapper::Up() const
  {
    return 1.0;
  }

  // Implementation of TracedRosenbrockWrapper.

  double
  TracedRosenbrockWrapper::operator()(std::vector<double> const& x) const
  {
    // Accumulate a newline-delimited string containing the whole trace,
    // and record that in our multiset.
    std::string stack;
    auto frames = boost::stacktrace::stacktrace();
    for (auto const& frame : frames) {
      stack += to_string(frame);
      stack += "\n";
    }

    auto it = stacks_.find(stack);
    if (it != stacks_.end()) {
      // this stack has already been seen
      (it->second) += 1; // increment the count
    } else {
      // this stack has not been seen before.
      // Record it with a count of 1.
      stacks_.insert({stack, 1});
    }

    return rosenbrock(x[0], x[1]);
  }

  double
  TracedRosenbrockWrapper::Up() const
  {
    return 1.0;
  }

  std::ostream&
  operator<<(std::ostream& os, TracedRosenbrockWrapper const& w)
  {
    for (auto it = w.stacks_.cbegin(); it != w.stacks_.cend(); ++it) {
      os << it->first; // write out the stack
      os << "###ncalls " << it->second << '\n';
    }
    return os;
  }

}

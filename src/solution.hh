#ifndef PROFILED_FC_CPU_SOLUTION_HH
#define PROFILED_FC_CPU_SOLUTION_HH

#include "geometry.hh"

#include <iosfwd>

namespace pfc {
  struct solution {
    column_vector start;
    column_vector location;
    long index = -1;
    double start_value;
    double value;
    double tstart;
    double tstop;
    long nsteps = -1; // not all algorithms will fill this value
  };

  // solutions are sorted by the value: the smallest value is the obvious best
  // minimum so far. Note that the *smallest* value has the highest priority.
  inline bool
  operator<(solution const& a, solution const& b)
  {
    return a.value < b.value; // smaller value is higher priority
  }

  std::ostream& operator<<(std::ostream& os, solution const& s);

  inline
  long ndims(solution const& s)
  {
    return s.location.size();
  }
}

#endif
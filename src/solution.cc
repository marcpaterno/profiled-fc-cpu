#include "solution.hh"

#include "fmt/format.h"
#include <ostream>

namespace {
  inline std::string
  format_double(double x)
  {
    return fmt::format("{:.17e}", x);
  }
}

std::ostream&
pfc::operator<<(std::ostream& os, pfc::solution const& sol)
{
  auto delta = sol.start - sol.location;
  double dist = dlib::length(delta);
  os << sol.index << '\t' << format_double(sol.tstart) << '\t' << sol.start
     << '\t' << format_double(sol.start_value) << '\t'
     << format_double(sol.tstop) << '\t' << sol.location << '\t'
     << format_double(sol.value) << '\t' << dist << '\t' << sol.nsteps;
  return os;
}
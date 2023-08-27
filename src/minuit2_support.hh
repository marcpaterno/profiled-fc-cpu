#pragma once

#include <iosfwd>

// Forward declare this class to reduce header dependencies.
namespace ROOT::Minuit2 {
  class FunctionMinimum;
}

namespace pfc {
  void print_result_header(ROOT::Minuit2::FunctionMinimum const&,
                           std::ostream& os);

  void print_result(double toler,
                    ROOT::Minuit2::FunctionMinimum const&,
                    std::ostream& os);
}

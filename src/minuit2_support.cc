#include "Minuit2/FunctionMinimum.h"
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>

#include "minuit2_support.hh"

namespace pfc {
  void
  print_result_header(ROOT::Minuit2::FunctionMinimum const& m, std::ostream& os)
  {
    os << "valid\tncalls\tnstates\tfmin";
    std::size_t nparams = m.UserParameters().Parameters().size();
    for (std::size_t i = 0; i != nparams; ++i) {
      os << "\tp" << i;
    }
  }

  void
  print_result(ROOT::Minuit2::FunctionMinimum const& m, std::ostream& os)
  {
    constexpr auto max_precision{std::numeric_limits<long double>::digits10 +
                                 1};
    os << m.IsValid() << '\t' << m.NFcn() << '\t' << m.States().size() << '\t'
       << std::scientific << std::setprecision(max_precision) << m.Fval();
    for (auto const& param : m.UserParameters().Parameters()) {
      os << '\t' << param.Value();
    }
  }
}

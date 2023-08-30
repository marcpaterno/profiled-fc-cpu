#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "rosenbrock.hh"

#include <iostream>

int
main()
{
  pfc::TracedRosenbrockWrapper fcn;

  ROOT::Minuit2::MnMigrad minimizer(fcn, {-1.2, 1.0}, {1.e-3, 1.e-3});

  ROOT::Minuit2::FunctionMinimum result = minimizer(1000, 1.e-6);
  std::cout << fcn << '\n';
  return result.IsValid() ? 0 : 1;
}

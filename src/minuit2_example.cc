#include "Minuit2/FCNBase.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnUserParameters.h"

#include "minuit2_support.hh"
#include "rosenbrock.hh"

#include <iostream>

// Minuit2 requires that the "function" it miminzes be implemented as a
// callable class type that inherits from ROOT::Minuit2::FCNBase.
//
struct RosenbrockWrapper : ROOT::Minuit2::FCNBase {
  // Evalute the function at the location 'x'
  double operator()(std::vector<double> const& x) const override;

  // The value returned by 'Up' is used to determine the errors in the fitted
  // parameters. This is a pure virtual function in the base calss, and so we
  // must implement it. However, we do not care about the errors reported by
  // the minimizer; they are in no way relevant for our use of a minimizer.
  // This also means that the errors reported in a minimization result are
  // meaningless.
  double Up() const override;
};

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

int
main()
{
  // Create the callable object to be minimized.
  RosenbrockWrapper fcn;

  // The user parameters object tells Minuit2 about the starting values for our
  // parameters, and 'errors' in those parameters. Documentation in
  // MnUserParameters
  // (https://root.cern.ch/doc/master/MnUserParameters_8cxx_source.html) says
  // that the error values are related to a step size.
  //
  // ROOT::Minuit2::MnUserParameters user_params({2.0, 2.0}, {1.e-3, 1.e-3});
  // ROOT::Minuit2::MnMigrad minimizer(fcn, user_params);

  // This seems to be the minimalist way to create the minimizer object. The
  // construction below is equivalent to the multi-step process that is
  // commented out, above.
  ROOT::Minuit2::MnMigrad minimizer(
    fcn,             // the function to be minimized
    {2.0, 2.0},      // the starting parameter values
    {1.e-3, 1.e-3}); // startig parameter 'errors'

  // max_calls is the maximum number of function calls allowed.
  // The default, 0, sets this maximum to 200 + 100 * npar + 5 * npar**2,
  // where npar is the number of parameters in the function being minimzed.
  // For the 2-parameter Rosenbrock function, this is 420, which is far too
  // small.
  unsigned int const max_calls = 1000000;

  // toler is the accuracy to which we want to find the minimum; that is,
  // the minimization will stop when the minimizer thinks the distance to the
  // true minimum is toler or less. Note that the parameter actually passed to
  // the function call operator of the minimizer is calculated from toler; this
  // calculation reflects the odd specification of that parameter. See the
  // documentation at
  // https://root.cern.ch/doc/master/classROOT_1_1Minuit2_1_1MnApplication.html#a8908d50d5d4f7f011b94bd10e51eacf7
  // for details.
  double toler = 1.e-6;
  ROOT::Minuit2::FunctionMinimum result = minimizer(max_calls, toler / 1.e-3);
  pfc::print_result_header(result, std::cout);
  std::cout << '\n';
  pfc::print_result(result, std::cout);
  std::cout << '\n';
}

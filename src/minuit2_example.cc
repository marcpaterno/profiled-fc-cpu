#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnUserParameters.h"

#include "minuit2_support.hh"
#include "rosenbrock.hh"

#include <iostream>

bool
do_loop(pfc::RosenbrockWrapper const& fcn, bool print_header, double toler)
{
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
    {-1.2, 1.0},     // the starting parameter values
    {1.e-3, 1.e-3}); // starting parameter 'errors'

  // max_calls is the maximum number of function calls allowed.
  // The default, 0, sets this maximum to 200 + 100 * npar + 5 * npar**2,
  // where npar is the number of parameters in the function being minimzed.
  // For the 2-parameter Rosenbrock function, this is 420. We want to make
  // sure we don't stop trying to converge based in this number, so we use
  // a very large value.
  unsigned int const max_calls = 1000 * 1000;

  // toler is the accuracy to which we want to find the minimum; that is,
  // the minimization will stop when the minimizer thinks the distance to the
  // true minimum is toler or less. Note that the parameter actually passed to
  // the function call operator of the minimizer is calculated from toler; this
  // calculation reflects the odd specification of that parameter. See the
  // documentation at
  // https://root.cern.ch/doc/master/classROOT_1_1Minuit2_1_1MnApplication.html#a8908d50d5d4f7f011b94bd10e51eacf7
  // for details.
  ROOT::Minuit2::FunctionMinimum result = minimizer(max_calls, toler / 1.e-3);
  if (print_header) {
    std::cout << "toler\t";
    pfc::print_result_header(result, std::cout);
    std::cout << '\n';
  }

  pfc::print_result(toler, result, std::cout);
  std::cout << '\n';
  return result.IsValid();
}

int
main()
{
  // Create the callable object to be minimized.
  pfc::RosenbrockWrapper fcn;

  // Run the minimizer with a successively tighter tolerance requirement.
  // Note that we are *not* reusing the same minimizer; it keeps state between
  // calls, and would give us incorrect information about the number of calls
  // made. We will stop looping when the minimizer thinks it has not converged,
  // or when we reach the specified minimum tolerance.
  double toler = 1.e-3; // initial tolerance
  double const minimum_tolerance = 1.e-15;

  bool keep_going =
    true; // will be set to false when the minimization does not converge
  bool first_loop = true;
  while (toler > minimum_tolerance && keep_going) {
    keep_going = do_loop(fcn, first_loop, toler);
    // prepare for next loop
    toler /= 2;
    first_loop = false;
  }
}

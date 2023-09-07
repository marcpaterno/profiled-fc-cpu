#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnUserParameters.h"

#include "minuit2_support.hh"
#include "rastrigin.hh"

#include <iostream>
#include <string>

bool
do_loop(pfc::RastriginWrapper const& fcn,
        std::vector<double> const& starting_point,
        bool print_header,
        double toler)
{
  ROOT::Minuit2::MnMigrad minimizer(fcn, starting_point, {1.e-3, 1.e-3, 1.e-3});
  unsigned int const max_calls = 1000 * 1000;

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
main(int argc, char* argv[])
{
  // We require 3 input arguments specifying the starting point.
  if (argc != 4) {
    std::cerr << "Please supply 3 numeric arguments, specifying the starting "
                 "point for the search.";
    return 1;
  }

  // Turn the input arguments from strings to doubles.
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<double> starting_point(args.size());
  for (std::size_t i = 0; i != args.size(); ++i) {
    starting_point[i] = std::stod(args[i]);
  }

  pfc::RastriginWrapper fcn;
  double toler = 1.e-3;
  double const minimum_tolerance = 1.e-15;

  bool keep_going = true;
  bool first_loop = true;
  while (toler > minimum_tolerance && keep_going) {
    keep_going = do_loop(fcn, starting_point, first_loop, toler);
    toler /= 2;
    first_loop = false;
  }
}

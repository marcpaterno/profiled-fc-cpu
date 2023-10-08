#include "oneapi/tbb.h"

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <vector>

// This is a program that demonstrates TBB works in this environment.
//
using oneapi::tbb::blocked_range;
using oneapi::tbb::parallel_reduce;
using numeric_type = long long;

std::mutex hideous_global_cerr_mutex;

numeric_type
sum_blocked_range(blocked_range<numeric_type> const& r, numeric_type init)
{
  { // scope to manage lifetime of lock
    std::lock_guard<std::mutex> guard(hideous_global_cerr_mutex);
    std::cerr << "working on range from " << r.begin() << " to " << r.end()
              << '\n';
  }
  for (numeric_type v = r.begin(); v != r.end(); v++) {
    init += v;
  }
  return init;
}

numeric_type
sum_ints(numeric_type x, numeric_type y)
{
  { // scope to manage lifetime of lock
    std::lock_guard<std::mutex> guard(hideous_global_cerr_mutex);
    std::cerr << "adding " << x << " and " << y << '\n';
  }
  return x + y;
};

int
main(int argc, char** argv)
{
  std::vector<std::string> args(argv + 1, argv + argc);
  if (args.empty()) {
    std::cerr << "Please supply an integer upper range\n";
    return 1;
  }
  numeric_type upper = std::atol(args[0].c_str());
  blocked_range<numeric_type> integers(1, upper, 1);
  // The type of the second argument (the accumulator) passed to parallel_reduce
  // is critical. oneTBB will use whatever type is specified as the accumulator;
  // if this type does not match the types being added, then overflows may lead
  // to undefined behavior.
  auto sum = parallel_reduce(integers, 0LL, sum_blocked_range, sum_ints);
  std::cout << "Sum: " << sum << '\n';
}

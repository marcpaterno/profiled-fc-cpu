#include "shared_result.hh"
#include "geometry.hh"
#include "solution.hh"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include <chrono>

using pfc::column_vector;
using pfc::shared_result;
using pfc::solution;

double
function(double x)
{
  return x * x;
}
inline double
now_in_milliseconds()
{
  using namespace std::chrono;
  auto const t = steady_clock::now().time_since_epoch();
  return duration<double>(t).count() * 1000.0;
}

TEST_CASE("not filled")
{
  shared_result solutions(1.e-6, 2);
  CHECK(solutions.empty());
  CHECK(solutions.num_results() == 0);

  solution s1;
  s1.start = column_vector({1.0});
  s1.tstart = now_in_milliseconds();
  s1.location = column_vector({0.5});
  s1.index = 1;
  s1.start_value = function(s1.start(0));
  s1.value = function(s1.location(0));
  s1.tstop = now_in_milliseconds();
  solutions.insert(s1);
  CHECK(!solutions.is_done());
  CHECK(!solutions.empty());
  CHECK(solutions.num_results() == 1);

  solution s2;
  s2.start = column_vector({4.0});
  s2.tstart = now_in_milliseconds();
  s2.location = column_vector({0.2});
  s2.index = 2;
  s2.start_value = function(s2.start(0));
  s2.value = function(s2.location(0));
  s2.tstop = now_in_milliseconds();
  solutions.insert(s2);
  CHECK(!solutions.is_done());
  CHECK(solutions.num_results() == 2);

  solution s3;
  s3.start = column_vector({0.5});
  s3.tstart = now_in_milliseconds();
  s3.location = column_vector({1.0e-8});
  s3.index = 3;
  s3.start_value = function(s3.start(0));
  s3.value = function(s3.location(0));
  s3.tstop = now_in_milliseconds();
  solutions.insert(s3);
  CHECK(solutions.is_done());
  CHECK(solutions.num_results() == 3);

  REQUIRE_THAT(solutions.best().value, Catch::Matchers::WithinAbs(0.0, 1.e-6));
}
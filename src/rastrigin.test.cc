#include "rastrigin.hh"
#include "catch2/catch_test_macros.hpp"
#include <numbers>

TEST_CASE("specific points give correct answer")
{
  REQUIRE(pfc::rastrigin({0.}) == 0.);
  REQUIRE(pfc::rastrigin({0., 0.}) == 0.);
  REQUIRE(pfc::rastrigin({0., 0., 0.}) == 0.);

  REQUIRE(pfc::rastrigin({1.0, 2.0}) == 5.0);
  REQUIRE(pfc::rastrigin({-1., 2., 3.}) == 14.0);
}

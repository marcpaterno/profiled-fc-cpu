#include "rosenbrock.hh"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("specific points give correct answer")
{
  REQUIRE(pfc::rosenbrock(1., 1.) == 0.);
  REQUIRE(pfc::rosenbrock(0., 0.) == 1.);
  REQUIRE(pfc::rosenbrock(2., 1.) == 901.);
}

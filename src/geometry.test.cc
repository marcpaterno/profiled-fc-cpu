#include "geometry.hh"
#include "catch2/catch_test_macros.hpp"

using pfc::column_vector;
using pfc::region;

TEST_CASE("line splitting")
{
  column_vector lower(1);
  lower(0) = 0.0;
  column_vector upper(1);
  upper(0) = 2.0;
  region line(lower, upper);
  CHECK(line.ndims() == 1);

  CHECK(pfc::detail::determine_split_dimension(line) == 0);

  auto [a, b] = line.split();
  // line is split in half
  CHECK(a.lower(0) == 0.0);
  CHECK(a.upper(0) == 1.0);
  CHECK(b.lower(0) == 1.0);
  CHECK(b.upper(0) == 2.0);
}

TEST_CASE("rectangle splitting")
{
  region rectangle({0.0, 0.0}, {1.0, 4.0});
  CHECK(rectangle.ndims() == 2);

  CHECK(pfc::detail::determine_split_dimension(rectangle) == 1);

  auto [a, b] = rectangle.split();
  CHECK(a.ndims() == 2);
  CHECK(b.ndims() == 2);

  // verify original
  REQUIRE(rectangle.lower(0) == 0.0);
  REQUIRE(rectangle.lower(1) == 0.0);
  REQUIRE(rectangle.upper(0) == 1.0);
  REQUIRE(rectangle.upper(1) == 4.0);

  // x-direction should be unchanged.
  CHECK(a.lower(0) == 0.0);
  CHECK(a.upper(0) == 1.0);
  CHECK(b.lower(0) == 0.0);
  CHECK(b.upper(0) == 1.0);

  // y-direction should be split in half.
  CHECK(a.lower(1) == 0.0);
  CHECK(a.upper(1) == 2.0);
  CHECK(b.lower(1) == 2.0);
  CHECK(b.upper(1) == 4.0);
}

TEST_CASE("square splitting")
{
  region square({0.0, 0.0}, {4.0, 4.0});
  CHECK(square.ndims() == 2);

  CHECK(pfc::detail::determine_split_dimension(square) == 0);

  auto [a, b] = square.split();
  CHECK(a.ndims() == 2);
  CHECK(b.ndims() == 2);

  // x-direction should be split in half.
  CHECK(a.lower(0) == 0.0);
  CHECK(a.upper(0) == 2.0);
  CHECK(b.lower(0) == 2.0);
  CHECK(b.upper(0) == 4.0);

  // y-direction should be unchanged.
  CHECK(a.lower(1) == 0.0);
  CHECK(a.upper(1) == 4.0);
  CHECK(b.lower(1) == 0.0);
  CHECK(b.upper(1) == 4.0);
}
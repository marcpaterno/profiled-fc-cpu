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

  CHECK(pfc::determine_split_dimension(line) == 0);

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

  CHECK(pfc::determine_split_dimension(rectangle) == 1);

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

  CHECK(pfc::determine_split_dimension(square) == 0);

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

TEST_CASE("splitting generations")
{
  region three_d({0.0, 0.0, 0.0}, {128.0, 256.0, 512.0});
  std::vector<region<pfc::column_vector>> original{three_d};
  std::vector<region<pfc::column_vector>> result = make_splits(7, original);
  CHECK(result.size() == 128);
  double const expected_volume = three_d.volume() / 128;
  for (auto const& r : result) {
    CHECK(r.volume() == expected_volume);
  }
}

TEST_CASE("random locations")
{
  region three_d({-10.0, -5.0, 10.0}, {0.0, 5.0, 20.0});
  // We are using srd::ranlux48 not because it is great, but because we want to
  // help make sure that any UniformRandomBitGenerator can be used.
  std::ranlux48 engine;
  CHECK(three_d.volume() == 1000.0);
  for (int i = 0; i < 1000; ++i) {
    auto location = pfc::random_point_within(three_d, engine);
    CHECK(pfc::within_region(location, three_d));
  }
}
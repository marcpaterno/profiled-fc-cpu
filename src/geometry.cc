#include "geometry.hh"

#include "Random123/ReinterpretCtr.hpp"
#include "Random123/array.h"
#include "Random123/threefry.h"

#include <ostream>
#include <stdexcept>

namespace pfc {
  class Generator {
  public:
    explicit Generator(uint32_t id1 = 0, uint32_t id2 = 0);
    double next_double();

  private:
    using G = r123::ReinterpretCtr<r123::Array8x32, r123::Threefry4x64>;
    G generator;
    G::key_type key;
    G::ctr_type ctr{{}}; // initialize with zeros
  };

  Generator::Generator(uint32_t id1, uint32_t id2) : key{{id1, id2}} {};
}

std::size_t
pfc::detail::determine_split_dimension(pfc::region const& r)
{
  double maxlength =
    r.width(0); // our default, until a bigger dimension is found.
  std::size_t split_index =
    0; // our default, until a bigger dimension is found.
  std::size_t const n = r.ndims();
  for (std::size_t i = 1; i < n; ++i) {
    if (r.width(i) > maxlength) {
      maxlength = r.width(i);
      split_index = i;
    }
  }
  return split_index;
}

// Implementation of region's member functions.
//
pfc::region::region(column_vector const& lo, column_vector const& up)
  : lower_(lo), upper_(up)
{
  if (lower_.size() != upper_.size())
    throw std::logic_error("Mismatched column lengths");
}

pfc::region::region(std::size_t ndim) : lower_(ndim), upper_(ndim) {}

std::size_t
pfc::region::ndims() const
{
  return lower_.size();
}

std::pair<pfc::region, pfc::region>
pfc::region::split() const
{
  auto const nd = ndims();
  std::pair<region, region> result(nd, nd);

  // Find dimension we will split.
  std::size_t split_dim = detail::determine_split_dimension(*this);
  double split_location = (lower_(split_dim) + upper_(split_dim)) / 2.0;
  region& a = result.first;
  region& b = result.second;

  // Do the split.
  a.lower_ = lower_;
  a.upper_ = upper_;
  b = a;
  a.lower_(split_dim) = lower_(split_dim);
  a.upper_(split_dim) = split_location;
  b.lower_(split_dim) = split_location;
  b.upper_(split_dim) = upper_(split_dim);

  return result;
}

double
pfc::region::width(std::size_t i) const
{
  return upper_(i) - lower_(i);
}

double
pfc::region::volume() const
{
  double vol = width(0);
  for (std::size_t i = 1; i != ndims(); ++i) {
    vol *= width(i);
  }
  return vol;
}

// Implementation of free functions
namespace pfc {
  std::ostream&
  operator<<(std::ostream& os, column_vector const& cv)
  {
    if (cv.size() == 0)
      return os;
    os << cv(0);

    for (std::size_t i = 1; i != cv.size(); ++i) {
      os << '\t' << cv(i);
    }
    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, bounds const& b)
  {
    os << '(' << b.lower << ") (" << b.upper << ')';
    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, region const& r)
  {
    os << '(' << r.lower_ << ") (" << r.upper_ << ')';
    return os;
  }

  std::vector<region>
  make_splits(int ngenerations, std::vector<region> const& regions)
  {
    assert(ngenerations >= 0);
    if (ngenerations == 0)
      return regions;

    std::vector<region> new_generation;
    new_generation.reserve(2 * regions.size());
    for (auto const& r : regions) {
      auto [a, b] = r.split();
      new_generation.push_back(a);
      new_generation.push_back(b);
    }
    return make_splits(ngenerations - 1, new_generation);
  }

  bool
  within_region(column_vector const& point, region const& r)
  {
    auto psize = point.size();
    auto rsize = r.ndims();
    if (psize != rsize)
      throw std::logic_error("Point dimensionality does not match region.");
    for (std::size_t i = 0; i != psize; ++i) {
      if (point(i) < r.lower(i))
        return false;
      if (point(i) > r.upper(i))
        return false;
    }
    return true;
  }

}
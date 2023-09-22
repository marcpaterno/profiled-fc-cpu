#include "geometry.hh"

#include <ostream>
#include <stdexcept>

// Helper functions in namespace to help make it clear they should not be used
// by client code.

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
  std::cerr << "splitting region: " << *this << "\nsplitting on index "
            << split_dim << "\nsplitting at location " << split_location
            << '\n';

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

  std::cerr << "split results are:\n\t" << result.first << "\n\t"
            << result.second << '\n';
  return result;
}

double
pfc::region::width(std::size_t i) const
{
  return upper_(i) - lower_(i);
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
      os << ", " << cv(i);
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
}
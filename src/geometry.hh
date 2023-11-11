#ifndef PROFILED_FC_CPU_GEOMETRY_HH
#define PROFILED_FC_CPU_GEOMETRY_HH

#include "dlib/matrix.h"
#include "fmt/format.h"

#include <iosfwd>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace pfc {

  // Use argument type column_vector for functions that want a runtime-sized
  // argument. Use argument type fixed_vector<N> for functions that want a
  // compile-time size. Note that the runtime size require heap memory
  // allocation.
  using column_vector = dlib::matrix<double, 0, 1>;

  template <int N>
  using fixed_vector = dlib::matrix<double, N, 1>;

  // Forward-declare region, which is defined below.
  template <typename VEC = pfc::column_vector>
  class region;

  template <typename VEC>
  std::vector<region<VEC>> make_splits(int ngenerations,
                                       std::vector<region<VEC>> const& regions);

  std::ostream& operator<<(std::ostream& os, column_vector const& cv);

  template <int N>
  std::ostream& operator<<(std::ostream& os, fixed_vector<N> const& cv);

  // Structure to supply the boundaries for the minimization search region.
  // This mostly exists to allow us to use structured binding on the return
  // value of make_bounds.
  struct bounds {
    explicit bounds(int dim) : lower(dim), upper(dim) {}
    column_vector lower;
    column_vector upper;
  };

  std::ostream& operator<<(std::ostream& os, bounds const& b);

  bounds make_bounds(int dim);

  // A region represents a region in an n-dimensional space (R^n).
  // A region is specified by using a column vector of lower bounds, and another
  // column vector (which must be the same length) of upper bounds.

  template <typename VEC>
  class region {
  public:
    using column_vector = VEC;
    // Create a region representing the space between the upper bounds and the
    // lower bounds. For each "slot" i in the column vectors, it is required
    // that lower(i) < upper(i); note the strict inequality. The column vectors
    // must be of the same length.
    region(column_vector const& lower, column_vector const& upper);

    // Create a region of the specified dimensionality, but with the boundaries
    // uninitialized.
    explicit region(std::size_t ndim);

    // Return the dimenstionality of the region.
    std::size_t ndims() const;

    // Subdivide *this into two distinct regions by splitting the largest
    // extent in half. If there are several extents all of which are the
    // same largest size, split the one with the lowest dimension index.
    std::pair<region, region> split() const;

    // Return the extent of the region in dimension i. It is required that i
    // be less than ndim().
    double width(std::size_t i) const;

    column_vector const& lower() const;
    column_vector const& upper() const;

    double lower(std::size_t i) const;
    double upper(std::size_t i) const;

    double& lower(std::size_t i);
    double& upper(std::size_t i);

    double volume() const;

  private:
    // Lower and upper must be of the same length.
    // Lower is the arrray of lower bounds of each dimension in the region;
    // upper is array of upper bounds. lower can also be thought of as the
    // lower corner, and upper as the upper, opposite corner.
    column_vector lower_;
    column_vector upper_;

    friend std::ostream& operator<<(std::ostream& os, region const& r);
  };

  // Return true if the given point is within the region (including on its
  // border), false otherwise.
  template <typename VEC>
  bool within_region(typename region<VEC>::column_vector const& point,
                     region<VEC> const& r);

  // Return a random point, drawn from a uniform distribution across the whole
  // region.
  template <typename VEC, typename URBG>
  region<VEC>::column_vector random_point_within(region<VEC> const& r);

  // Implementation details below.

  template <typename VEC>
  region<VEC>::column_vector const&
  region<VEC>::lower() const
  {
    return lower_;
  }

  template <typename VEC>
  region<VEC>::column_vector const&
  region<VEC>::upper() const
  {
    return upper_;
  }

  template <typename VEC>
  double
  region<VEC>::lower(std::size_t i) const
  {
    return lower_(i);
  }

  template <typename VEC>
  double
  region<VEC>::upper(std::size_t i) const
  {
    return upper_(i);
  }

  template <typename VEC>
  double&
  region<VEC>::lower(std::size_t i)
  {
    return lower_(i);
  }

  template <typename VEC>
  double&
  region<VEC>::upper(std::size_t i)
  {
    return upper_(i);
  }

  template <typename VEC>
  std::size_t
  determine_split_dimension(region<VEC> const& r)
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

  template <typename VEC>
  region<VEC>::region(region<VEC>::column_vector const& lo,
                      region<VEC>::column_vector const& up)
    : lower_(lo), upper_(up)
  {
    if (lower_.size() != upper_.size())
      throw std::logic_error("Mismatched column lengths");
  }

  template <typename VEC>
  region<VEC>::region(std::size_t ndim) : lower_(ndim), upper_(ndim)
  {}

  template <typename VEC>
  std::size_t
  region<VEC>::ndims() const
  {
    return lower_.size();
  }

  template <typename VEC>
  std::pair<region<VEC>, region<VEC>>
  region<VEC>::split() const
  {
    auto const nd = ndims();
    std::pair<region<VEC>, region<VEC>> result(nd, nd);

    // Find dimension we will split.
    std::size_t split_dim = determine_split_dimension(*this);
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

  template <typename VEC>
  double
  region<VEC>::width(std::size_t i) const
  {
    return upper_(i) - lower_(i);
  }

  template <typename VEC>
  double
  region<VEC>::volume() const
  {
    double vol = width(0);
    for (std::size_t i = 1; i != ndims(); ++i) {
      vol *= width(i);
    }
    return vol;
  }

  template <int N>
  region<pfc::fixed_vector<N>>
  make_box_in_dim(double low, double high)
  {
    fixed_vector<N> lo;
    fixed_vector<N> hi;
    for (int i = 0; i < N; ++i) {
      lo(i) = low;
      hi(i) = high;
    }
    region<pfc::fixed_vector<N>> result(lo, hi);
    return result;
  }

  inline region<pfc::column_vector>
  make_box_in_n_dim(int ndim, double low, double high)
  {
    typename pfc::column_vector lo(ndim);
    typename pfc::column_vector hi(ndim);
    for (int i = 0; i < ndim; ++i) {
      lo(i) = low;
      hi(i) = high;
    }
    region<pfc::column_vector> result(lo, hi);
    return result;
  }

  template <typename VEC>
  std::size_t determine_split_dimension(pfc::region<VEC> const& r);

  template <typename URBG, typename VEC>
    requires std::uniform_random_bit_generator<URBG>
  region<VEC>::column_vector
  random_point_within(region<VEC> const& r, URBG& e)
  {
    typename region<VEC>::column_vector result(r.ndims());
    // We need to make sure we do not generated a random value of 0, because
    // that would give a point on the boundary rather than inside the volume.
    std::uniform_real_distribution flat(std::numeric_limits<double>::min(),
                                        1.0);
    for (std::size_t i = 0; i != r.ndims(); ++i) {
      double const val = flat(e) * r.width(i) + r.lower(i);
      result(i) = val;
    }
    return result;
  }

  inline std::ostream&
  operator<<(std::ostream& os, column_vector const& cv)
  {
    if (cv.size() == 0)
      return os;
    os << fmt::format("{:.17e}", cv(0));

    for (std::size_t i = 1; i != cv.size(); ++i) {
      os << '\t' << fmt::format("{:.17e}", cv(i));
    }
    return os;
  }

  inline std::ostream&
  operator<<(std::ostream& os, bounds const& b)
  {
    os << '(' << b.lower << ") (" << b.upper << ')';
    return os;
  }

  template <typename VEC>
  std::ostream&
  operator<<(std::ostream& os, region<VEC> const& r)
  {
    os << '(' << r.lower_ << ") (" << r.upper_ << ')';
    return os;
  }

  template <typename VEC>
  std::vector<region<VEC>>
  make_splits(int ngenerations, std::vector<region<VEC>> const& regions)
  {
    assert(ngenerations >= 0);
    if (ngenerations == 0)
      return regions;

    std::vector<region<VEC>> new_generation;
    new_generation.reserve(2 * regions.size());
    for (auto const& r : regions) {
      auto [a, b] = r.split();
      new_generation.push_back(a);
      new_generation.push_back(b);
    }
    return make_splits(ngenerations - 1, new_generation);
  }

  template <typename VEC>
  bool
  within_region(typename region<VEC>::column_vector const& point,
                region<VEC> const& r)
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

#endif

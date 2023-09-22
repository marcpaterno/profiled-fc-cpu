#ifndef PROFILED_FC_CPU_GEOMETRY_HH
#define PROFILED_FC_CPU_GEOMETRY_HH

#include "dlib/matrix.h"
#include <iosfwd>
#include <utility>

namespace pfc {

  // We have written the rastrigin function to be callable with a vector of
  // arbitrary length, so we use the dynamic-sized version of column vector.
  using column_vector = dlib::matrix<double, 0, 1>;

  std::ostream& operator<<(std::ostream& os, column_vector const& cv);

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

  class region {
  public:
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

  private:
    // Lower and upper must be of the same length.
    // Lower is the arrray of lower bounds of each dimension in the region;
    // upper is array of upper bounds. lower can also be thought of as the
    // lower corner, and upper as the upper, opposite corner.
    column_vector lower_;
    column_vector upper_;

    friend std::ostream& operator<<(std::ostream& os, region const& r);
  };

  // Implementation details below.

  inline column_vector const&
  region::lower() const
  {
    return lower_;
  }

  inline column_vector const&
  region::upper() const
  {
    return upper_;
  }

  inline double
  region::lower(std::size_t i) const
  {
    return lower_(i);
  }

  inline double
  region::upper(std::size_t i) const
  {
    return upper_(i);
  }

  inline double&
  region::lower(std::size_t i)
  {
    return lower_(i);
  }

  inline double&
  region::upper(std::size_t i)
  {
    return upper_(i);
  }

  // The following should not be used outside the implementaton of geometry.cc.
  // They are here only to allow testing.
  namespace detail {
    std::size_t determine_split_dimension(pfc::region const& r);
  }

}

#endif
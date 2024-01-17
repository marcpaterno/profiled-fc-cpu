#include "shared_result.hh"

#include <algorithm>
#include <ostream>

namespace pfc {

  shared_result::shared_result(double desired_min, std::size_t max_results)
    : desired_min_(desired_min), max_results_(max_results)
  {}

  bool
  shared_result::is_sorted() const
  {
    return num_results_ > max_results_;
  }

  void
  shared_result::sort()
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    std::sort(results_.begin(), results_.end());
  }

  void
  shared_result::insert(solution s)
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    num_results_ += 1;
    s.index = num_results_;
    if (s.value < desired_min_)
      done_ = true;

    if (results_.empty()) {
      results_.push_back(s);
      return;
    }

    if (is_sorted()) {
      // Our vector of solutions is already sorted.
      // If s is not better than the worst, forget it.
      if (results_.back() < s) {
        return;
      }

      // Otherwise, insert it and drop the last.
      auto i = std::lower_bound(results_.begin(), results_.end(), s);
      results_.insert(i, s);
      results_.pop_back();
      return;
    }

    if (num_results_ == max_results_) {
      // solution s will "fill" our vector. Record it, and then sort the vector.
      // It will be kept sorted from here on.
      results_.push_back(s);
      std::sort(results_.begin(), results_.end());
      return;
    }

    // If we are here, then we have not yet "filled" the vector; push it onto
    // the vector.
    results_.push_back(s);
  }

  solution
  shared_result::best() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);

    if (num_results_ >= max_results_) {
      // The vector is sorted
      return results_[0];
    }

    // If the vector is not sorted, find the best
    return *std::min_element(results_.begin(), results_.end());
  }

  bool
  shared_result::is_done(long max_attempts) const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return done_ || (num_results_ > max_attempts);
  }

  std::vector<solution>
  shared_result::solutions() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return results_;
  }

  long
  shared_result::num_attempts() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return num_results_;
  }

  bool
  shared_result::empty() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return results_.empty();
  }

  void
  shared_result::print_report(std::ostream& os) const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    pfc::print_report(results_, os);
  }

  void
  print_report(std::vector<solution> const& results, std::ostream& os)
  {
    if (results.empty()) {
      return;
    }

    // Every starting point and solution has the same 'size', which is the
    // dimenstionality of the function we're minimizing.
    auto ndim = results.front().location.size();

    // Print a header for the data.
    os << "idx\ttstart\t";
    for (long i = 0; i != ndim; ++i)
      os << 's' << i << '\t';
    os << "fs\ttstop\t";
    for (long i = 0; i != ndim; ++i)
      os << 'x' << i << '\t';
    os << "min\tdist\tnsteps\n";

    for (auto const& result : results) {
      os << result << '\n';
    }
  }
} // namespace pfc

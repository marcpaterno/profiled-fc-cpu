#include "shared_result.hh"

#include <algorithm>
#include <ostream>

namespace pfc {

  shared_result::shared_result(double desired_min, std::size_t max_results)
    : desired_min_(desired_min), max_results_(max_results)
  {}

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

    if (num_results_ > max_results_) {
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
  shared_result::is_done() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return done_;
  }

  std::vector<solution>
  shared_result::to_vector()
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    return results_;
  }
} // namespace pfc

#include "shared_result.hh"

#include <ostream>

namespace pfc {

  shared_result::shared_result(double desired_min) : desired_min_(desired_min)
  {}

  void
  shared_result::insert(solution s)
  {
    // Maybe we should be inspecting 's' and deciding whether we are done based
    // on it? We do not know what our real strategy for declaring that we are
    // done will be.
    std::scoped_lock<std::mutex> lock(guard_results_);
    num_results_ += 1;

    s.index = num_results_;
    results_.push(s);
  }

  solution
  shared_result::best() const
  {
    std::scoped_lock<std::mutex> lock(guard_results_);
    solution result = results_.top();
    return result;
  }

  bool
  shared_result::is_done() const
  {
    auto current_best = best();
    return current_best.value < desired_min_;
  }

  std::vector<solution>
  shared_result::to_vector()
  {
    std::vector<solution> result;
    std::scoped_lock<std::mutex> lock(guard_results_);
    while (!results_.empty()) {
      result.push_back(results_.top());
      results_.pop();
    }
    return result;
  }
} // namespace pfc

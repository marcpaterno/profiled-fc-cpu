#ifndef PROFILED_FC_CPU_SHARED_RESULT_HH
#define PROFILED_FC_CPU_SHARED_RESULT_HH

#include "solution.hh"

#include <iosfwd>
#include <mutex>
#include <queue>
#include <vector>

namespace pfc {
  // shared_result is a container for attempted solutions of a minimization
  // problem. It is suitable to be used from multiple threads; it contains
  // simple internal locking to prevent race conditions.
  class shared_result {
  public:
    explicit shared_result(double desired_min);

    // Make sure we can neither copy or move a shared_result.
    shared_result(shared_result const&) = delete;
    shared_result& operator=(shared_result const&) = delete;
    shared_result(shared_result&&) = delete;
    shared_result& operator=(shared_result&&) = delete;

    // Insert a copy of sol into the shared result.
    // We take the argument by value because we want to make the copy.
    void insert(solution sol);

    // Obtain a copy of the best result thus far
    solution best() const;

    // Check whether we are done or not. The current implementation is very
    // naive; we are done when the best solution has found a local minimum with
    // value less than the value of desired_min used to configure the
    // shared_result object.
    bool is_done() const;

    friend std::ostream& operator<<(std::ostream& os, shared_result const& r);

    // Return a vector containing the priority-sorted solutions. Note that this
    // member function is non-const because it empties the priority queue during
    // the process of constructing the vector.
    std::vector<solution> to_vector();

  private:
    std::mutex mutable guard_results_;
    std::priority_queue<solution> results_;
    long num_results_;
    double const desired_min_;
  }; // shared_result

} // namespace pfc

#endif
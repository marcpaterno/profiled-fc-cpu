#ifndef PROFILED_FC_CPU_SHARED_RESULT_HH
#define PROFILED_FC_CPU_SHARED_RESULT_HH

#include "solution.hh"

#include <iosfwd>
#include <limits>
#include <mutex>
#include <vector>

namespace pfc {
  // shared_result is a container for attempted solutions of a minimization
  // problem. It is suitable to be used from multiple threads; it contains
  // simple internal locking to prevent race conditions.
  //
  // shared_result does not contain the code to do the minimization; it is only
  // a repository for results and a tool for determining whether one of the
  // results is "good enough".
  class shared_result {
  public:
    shared_result(double desired_min, std::size_t max_results);

    // Make sure we can neither copy or move a shared_result.
    // Since they are potentially large, we do not want to accidentally pass
    // them around.
    shared_result(shared_result const&) = delete;
    shared_result& operator=(shared_result const&) = delete;
    shared_result(shared_result&&) = delete;
    shared_result& operator=(shared_result&&) = delete;

    // Return true if the collection is sorted, false if it is not.
    bool is_sorted() const;

    // Sort the collection of solutions.
    void sort();

    // Insert a copy of sol into the shared result.
    // We take the argument by value because we want to make the copy.
    void insert(solution sol);

    // Obtain a copy of the best result thus far.
    solution best() const;

    // Check whether we are done or not. The current implementation is very
    // naive; we are done when the best solution has found a local minimum with
    // value less than the value of desired_min used to configure the
    // shared_result object.
    bool is_done(long num_attempts = std::numeric_limits<long>::max()) const;

    // Write out all the contained solutions in a format suitable for automated
    // processing.
    friend std::ostream& operator<<(std::ostream& os, shared_result const& r);

    // Return a copy of the contained solutions.
    std::vector<solution> solutions() const;

    // Report how many minimization attempts have been done.
    long num_attempts() const;

    // Report whether we have any solutions.
    bool empty() const;

    // Print report output to the given stream. This output is suitable for
    // machine analysis, but may not be very good for human reading.
    void print_report(std::ostream& os) const;

  private:
    std::mutex mutable guard_results_;
    std::vector<solution> results_;
    long num_results_ = 0;
    double const desired_min_;
    bool done_ = false;
    std::size_t max_results_;
  };

  void print_report(std::vector<solution> const& solutions, std::ostream& os);
} // namespace pfc

#endif

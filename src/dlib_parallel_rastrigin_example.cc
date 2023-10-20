#include "geometry.hh"
#include "rastrigin.hh"

#include "dlib/optimization.h"
#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <utility>

namespace pfc {
  struct solution {
    column_vector start;
    column_vector location;
    double start_value = std::numeric_limits<double>::quiet_NaN();
    double value = std::numeric_limits<double>::quiet_NaN();
  };

  // solutions are sorted by the value: the smallest value is the obvious best
  // minimum so far. Note that the *smallest* value has the highest priority.
  inline bool
  operator<(solution const& a, solution const& b)
  {
    return a.value > b.value; // smaller value is higher priority
  }

  inline std::ostream&
  operator<<(std::ostream& os, solution const& sol)
  {
    os << sol.start << '\t' << sol.start_value << '\t' << sol.location << '\t'
       << sol.value;
    return os;
  }

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
    void insert(solution const& sol);

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
    std::mutex mutable guard_vector_;
    std::priority_queue<solution> results_;
    double desired_min_;
  }; // shared_result

  shared_result::shared_result(double desired_min) : desired_min_(desired_min)
  {}

  void
  shared_result::insert(solution const& s)
  {
    // Maybe we should be inspecting 's' and deciding whether we are done based
    // on it? We do not know what our real strategy for declaring that we are
    // done will be.
    std::scoped_lock<std::mutex> lock(guard_vector_);
    results_.push(s);
  }

  solution
  shared_result::best() const
  {
    std::scoped_lock<std::mutex> lock(guard_vector_);
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
    std::scoped_lock<std::mutex> lock(guard_vector_);
    while (!results_.empty()) {
      result.push_back(results_.top());
      results_.pop();
    }
    return result;
  }
} // namespace pfc

using namespace pfc;

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rastrigin_dlib_wrapper(column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::rastrigin({begin(x), end(x)});
}

column_vector
do_one_minimization(column_vector const& starting_point)
{
  column_vector location{starting_point};
  dlib::find_min_using_approximate_derivatives(
    dlib::bfgs_search_strategy(),
    dlib::objective_delta_stop_strategy(1.0e-6),
    rastrigin_dlib_wrapper,
    location,
    -1.0); // we choose a negative value because our function is non-negative
  // location is the estimated location of the minimum.
  return location;
}

inline region
box_in_n_dim(int ndim, double low, double high)
{
  column_vector lo(ndim);
  column_vector hi(ndim);
  for (int i = 0; i < ndim; ++i) {
    lo(i) = low;
    hi(i) = high;
  }
  region result(lo, hi);
  return result;
}

void
do_all_work(long ndim, pfc::shared_result& solutions)
{
  // The task group is what we use to schedule tasks to run.
  oneapi::tbb::task_group tasks;

  // region const starting_point_volume({-10.0, -10.0, -10.0, -10.0, -10.0,
  // -10.0, -10.0}, {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0});

  region const starting_point_volume = box_in_n_dim(ndim, -10.0, 10.0);
  std::mutex protect_engine;
  std::mt19937 engine;

  int const num_starting_points = oneapi::tbb::info::default_concurrency();
  for (int i = 0; i != num_starting_points; ++i) {

    // find_with_continuation is the closure object we execute in each TBB task.
    // It works by:
    //    1. calling the local minimization function for the given starting
    //       point.
    //    2. recording the resulting minimum in the shared solution.
    //    3. if the shared solution says we are not done, generate a new
    //       starting point and call the continuation (which will be the same
    //       function).
    // We pass the argument starting_point by volume because we need the
    // function to have its own copy of the column_vector.
    auto find_with_continuation =
      [&solutions, &starting_point_volume, &protect_engine, &engine, &tasks](
        auto& continuation) -> void {
      column_vector starting_point;
      { // scope to manage lifetime of the lock
        std::scoped_lock<std::mutex> lock(protect_engine);
        starting_point =
          pfc::random_point_within(starting_point_volume, engine);
      }
      solution result;
      result.start = starting_point;
      result.start_value = rastrigin_dlib_wrapper(starting_point);
      column_vector const local_min = do_one_minimization(starting_point);
      result.location = local_min;
      result.value = rastrigin_dlib_wrapper(local_min);

      solutions.insert(result);
      // If we don't have a good enough solution yet, then keep going.
      // Note that it is possible that, in a single thread, we will observe
      // the following:
      //      1. is_done returns false
      //      2. another thread finishes a task, records the result, and is_done
      //         is now true.
      //      3. the original thread continues and schedules another task.
      //
      // Since this does not lead to an error (just a small amount of wasted
      // effort) we are not currently concerned with this issue.
      if (!solutions.is_done()) {
        tasks.run([=]() { continuation(continuation); });
      }
    };
    // We have to give a callable with no arguments to tasks.run, so we need
    // another lambda expression that captures all the arguments to be used
    // for the call int find_with_continuation.
    tasks.run([=]() { find_with_continuation(find_with_continuation); });
  }
  // Wait for all the tasks in the group to finish.
  tasks.wait();
}

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Please specify the number of dimensions to use\n";
    return 1;
  }
  long const ndim = std::stol(argv[1]);
  // Create shared state for answer.
  // We're done when we have found a minimum with a value < 1.0e-6.
  pfc::shared_result solutions(1.0e-6);

  do_all_work(ndim, solutions);
  auto results = solutions.to_vector();
  if (results.empty()) {
    std::cerr << "No solutions were found!\n";
    return 1;
  }

  // Print a header for the data.
  for (long i = 0; i != ndim; ++i)
    std::cout << 's' << i << '\t';
  std::cout << "fs\t";
  for (long i = 0; i != ndim; ++i)
    std::cout << 'x' << i << '\t';
  std::cout << "min\n";

  for (auto const& result : results) {
    std::cout << result << '\n';
  }
}
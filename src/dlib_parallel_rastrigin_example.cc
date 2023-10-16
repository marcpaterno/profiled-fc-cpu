#include "geometry.hh"
#include "rastrigin.hh"

#include "dlib/optimization.h"
#include "tbb/task_group.h"

#include <iostream>
#include <limits>
#include <mutex>
#include <utility>

namespace pfc {
  struct solution {
    column_vector location;
    double value = std::numeric_limits<double>::quiet_NaN();
  };

  // solutions are sorted by the value: the smallest value is the obvious best
  // minimum so far. Note that the *smallest* value has the highest priority.
  inline bool
  operator<(solution const& a, solution const& b)
  {
    return a.value > b.value; // smaller value is higher priority
  }

  class shared_result {
  public:
    explicit shared_result(double desired_min);
    // Insert a copy of sol into the shared result
    void insert(solution const& sol);

    // Obtain a copy of the best result thus far
    solution best() const;

    // Check whether we are done or not.
    bool is_done() const;

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
    std::lock_guard<std::mutex> const lock(guard_vector_);
    results_.push(s);
  }

  solution
  shared_result::best() const
  {
    std::lock_guard<std::mutex> const lock(guard_vector_);
    return results_.top();
  }

  bool
  shared_result::is_done() const
  {
    std::lock_guard<std::mutex> const lock(guard_vector_);
    return best().value < desired_min_;
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

int
main()
{
  // The task group is what we use to schedule tasks to run.
  oneapi::tbb::task_group tasks;

  // Create shared state for answer.
  // We're done when we have found a minimum with a value < 1.0e-6.
  pfc::shared_result solutions(1.0e-6);

  region const starting_point_volume({-4.0, -2.5, -1.0}, {1.0, 2.5, 4.0});
  std::mt19937 engine;

  int const num_starting_points = 5;
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
      [&solutions, &starting_point_volume, &engine, &tasks](
        column_vector starting_point, auto continuation) -> void {
      column_vector const local_min = do_one_minimization(starting_point);
      solutions.insert({local_min, rastrigin_dlib_wrapper(local_min)});
      // If we don't have a good enough solution yet, then keep going.
      if (!solutions.is_done()) {
        auto new_starting_point =
          pfc::random_point_within(starting_point_volume, engine);
        tasks.run(continuation(new_starting_point, continuation));
      }
    };
    tasks.run(find_with_continuation(
      pfc::random_point_within(starting_point_volume, engine),
      find_with_continuation));
  }
  // Wait for all the tasks in the group to finish.
  // When they have we are done.

  // Print out the result
}
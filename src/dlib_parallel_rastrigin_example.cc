#include "geometry.hh"
#include "rastrigin.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "dlib/optimization.h"
#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <utility>

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rastrigin_dlib_wrapper(pfc::column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::rastrigin({begin(x), end(x)});
}

inline double
now_in_milliseconds()
{
  using namespace std::chrono;
  auto const t = steady_clock::now().time_since_epoch();
  return duration<double>(t).count() * 1000.0;
}

pfc::solution
do_one_minimization(pfc::column_vector const& starting_point)
{
  pfc::solution result;
  result.start = starting_point;
  result.start_value = rastrigin_dlib_wrapper(starting_point);
  result.tstart = now_in_milliseconds();

  // the minimization routine will write the answer directly into
  // result.location so no extra copying is needed.
  result.location = starting_point;
  dlib::find_min_using_approximate_derivatives(
    dlib::bfgs_search_strategy(),
    dlib::objective_delta_stop_strategy(1.0e-6),
    rastrigin_dlib_wrapper,
    result.location,
    -1.0); // we choose a negative value because our function is non-negative
  // result.location is the estimated location of the minimum.
  result.tstop = now_in_milliseconds();
  result.value = rastrigin_dlib_wrapper(result.location);
  return result;
}

inline pfc::region
make_box_in_n_dim(int ndim, double low, double high)
{
  pfc::column_vector lo(ndim);
  pfc::column_vector hi(ndim);
  for (int i = 0; i < ndim; ++i) {
    lo(i) = low;
    hi(i) = high;
  }
  pfc::region result(lo, hi);
  return result;
}

void
do_all_work(long ndim, pfc::shared_result& solutions)
{
  // The task group is what we use to schedule tasks to run.
  oneapi::tbb::task_group tasks;

  pfc::region const starting_point_volume =
    make_box_in_n_dim(ndim, -10.0, 10.0);
  std::mutex protect_engine;
  std::mt19937 engine;

  int const num_starting_points = oneapi::tbb::info::default_concurrency();
  for (int i = 0; i != num_starting_points; ++i) {

    // find_with_continuation is the closure object we execute in each TBB
    // task. It works by:
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
      pfc::column_vector starting_point;
      { // scope to manage lifetime of the lock
        std::scoped_lock<std::mutex> lock(protect_engine);
        starting_point =
          pfc::random_point_within(starting_point_volume, engine);
      }
      pfc::solution result = do_one_minimization(starting_point);

      solutions.insert(result);
      // If we don't have a good enough solution yet, then keep going.
      // Note that it is possible that, in a single thread, we will observe
      // the following:
      //      1. is_done returns false
      //      2. another thread finishes a task, records the result, and
      //      is_done
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
  std::cout << "idx\ttstart\t";
  for (long i = 0; i != ndim; ++i)
    std::cout << 's' << i << '\t';
  std::cout << "fs\ttstop\t";
  for (long i = 0; i != ndim; ++i)
    std::cout << 'x' << i << '\t';
  std::cout << "min\tdist\n";

  for (auto const& result : results) {
    std::cout << result << '\n';
  }
}
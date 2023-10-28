#ifndef PROFILED_FC_CPU_MINIMIZERS_HH
#define PROFILED_FC_CPU_MINIMIZERS_HH

#include "geometry.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "dlib/optimization.h"
#include "tbb/task_group.h"

#include <chrono>

namespace pfc {

  // Return number of milliseconds since the epoch.
  inline double
  now_in_milliseconds()
  {
    using namespace std::chrono;
    auto const t = steady_clock::now().time_since_epoch();
    return duration<double>(t).count() * 1000.0;
  }

  template <typename FUNC>
  pfc::solution
  do_one_minimization(FUNC const& f, pfc::column_vector const& starting_point)
  {
    pfc::solution result;
    result.start = starting_point;
    result.start_value = f(starting_point);
    result.tstart = now_in_milliseconds();

    // the minimization routine will write the answer directly into
    // result.location so no extra copying is needed.
    result.location = starting_point;
    auto [f_value, nsteps] = dlib::find_min_using_approximate_derivatives(
      dlib::bfgs_search_strategy(),
      dlib::objective_delta_stop_strategy(1.0e-6),
      f,
      result.location,
      -1.0); // we choose a negative value because our function is non-negative
    // result.location is the estimated location of the minimum.
    result.tstop = now_in_milliseconds();
    result.value = f_value;
    result.nsteps = nsteps;
    return result;
  }

  // ParallelMinimizer is a callable object designed to be executed through
  // tasks in a TBB task group. It works by:
  //    1. calling the local minimization function for the given starting
  //       point.
  //    2. recording the resulting minimum in the shared solution.
  //    3. if the shared solution says we are not done, generate a new
  //       starting point and call the continuation (which will be the same
  //       function).
  template <typename FUNC>
  struct ParallelMinimizer {
    FUNC& func;
    pfc::shared_result& solutions;
    pfc::region const& starting_point_volume;
    std::mutex& protect_engine;
    std::mt19937& engine;
    oneapi::tbb::task_group& tasks;

    ParallelMinimizer(FUNC& function_to_minimize,
                      pfc::shared_result& sol,
                      pfc::region const& spv,
                      std::mutex& pe,
                      std::mt19937& eng,
                      oneapi::tbb::task_group& tsks)
      : func(function_to_minimize)
      , solutions(sol)
      , starting_point_volume(spv)
      , protect_engine(pe)
      , engine(eng)
      , tasks(tsks)
    {}

    template <typename CONTINUATION>
    void
    operator()(CONTINUATION& continuation) const
    {
      pfc::column_vector starting_point;
      { // scope to manage lifetime of the lock
        std::scoped_lock<std::mutex> lock(protect_engine);
        starting_point =
          pfc::random_point_within(starting_point_volume, engine);
      }
      pfc::solution result = pfc::do_one_minimization(func, starting_point);

      solutions.insert(result);
      // If we don't have a good enough solution yet, then keep going.
      // Note that it is possible that, in a single thread, we will observe
      // the following:
      //      1. is_done returns false
      //      2. another thread finishes a task, records the result, and
      //         is_done is now true.
      //      3. the original thread continues and schedules another task.
      //
      // Since this does not lead to an error (just a small amount of wasted
      // effort) we are not currently concerned with this issue.
      if (!solutions.is_done()) {
        tasks.run([continuation]() { continuation(continuation); });
      }
    }
  };
}
#endif
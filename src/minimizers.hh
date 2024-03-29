#ifndef PROFILED_FC_CPU_MINIMIZERS_HH
#define PROFILED_FC_CPU_MINIMIZERS_HH

#include "callable_traits.hh"
#include "geometry.hh"
#include "protected_engine.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "dlib/optimization.h"
#include "tbb/task_group.h"

#include <chrono>

namespace pfc {

  // Forward declarations.
  double now_in_milliseconds();

  struct minimization_results;

  template <typename FUNC>
  solution do_one_minimization(FUNC const& f,
                               column_vector const& starting_point);

  template <std::uniform_random_bit_generator URBG,
            typename FUNC,
            typename REGION>
  struct ParallelMinimizer;

  template <typename FUNC>
  minimization_results find_global_minimum(
    FUNC&& func,
    long ndim,
    region<column_vector> const& starting_point_volume,
    int num_starting_points,
    double tolerance,
    long max_attempts = 1000000);

  template <typename FUNC, typename REGION>
  minimization_results find_global_minimum_fixed(
    FUNC&& func,
    int num_starting_points,
    REGION const& starting_point_volume,
    double tolerance,
    long max_attempts = 1000000);

  // Implementations below...

  // Return number of milliseconds since the epoch.
  inline double
  now_in_milliseconds()
  {
    using namespace std::chrono;
    auto const t = steady_clock::now().time_since_epoch();
    return duration<double>(t).count() * 1000.0;
  }

  // Struct representing the set of solutions from the global minimization
  // function.
  struct minimization_results {
    std::vector<solution> best_solutions; // The best solutions found
    long num_attempts; // The total number of local minimizations done
  };

  template <typename FUNC>
  solution
  do_one_minimization(FUNC const& f, column_vector const& starting_point)
  {
    solution result;
    result.start = starting_point;
    result.start_value = f(starting_point);
    result.tstart = now_in_milliseconds();

    // the minimization routine will write the answer directly into
    // result.location so no extra copying is needed.
    result.location = starting_point;
    auto [f_value, nsteps, steps] =
      dlib::find_min_using_approximate_derivatives(
        dlib::bfgs_search_strategy(),
        dlib::objective_delta_stop_strategy(1.0e-6),
        f,
        result.location,
        -1.0); // we choose a negative value because our function is
               // non-negative
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
  //       starting point keep trying.
  template <std::uniform_random_bit_generator URBG,
            typename FUNC,
            typename REGION>
  struct ParallelMinimizer {
    FUNC& func;
    shared_result& solutions;
    REGION const& starting_point_volume;
    protected_engine<URBG>& engine;
    long max_attempts;

    ParallelMinimizer(FUNC& function_to_minimize,
                      shared_result& sol,
                      REGION const& spv,
                      protected_engine<URBG>& eng,
                      long max_attempts = 1000000)
      : func(function_to_minimize)
      , solutions(sol)
      , starting_point_volume(spv)
      , engine(eng)
      , max_attempts(max_attempts)
    {}

    void
    operator()() const
    {
      // Loop until we have a good enough solution, or until we've used all our
      // attempts.
      // Note that if another task find a solution quickly enough, we may end
      // never entering the loop.

      while (!solutions.is_done(max_attempts)) {
        auto starting_point =
          random_point_within(starting_point_volume, engine);
        solution result = do_one_minimization(func, starting_point);
        solutions.insert(result);
      }
    }

    long
    num_attempts() const
    {
      return solutions.num_attempts();
    }
  };

  // This is the function that does all the minimization work.
  // It is a blocking function that schedules parallel work, and waits until
  // that work is done before returning.
  template <typename FUNC>
  minimization_results
  find_global_minimum(FUNC&& func,
                      long ndim,
                      region<column_vector> const& starting_point_volume,
                      int num_starting_points,
                      double tolerance,
                      long max_attempts)
  {
    shared_result solutions(tolerance, num_starting_points);
    // The task group is what we use to schedule tasks to run.
    oneapi::tbb::task_group tasks;

    // All our starting points will be generated within the region
    // 'starting_point_volume'. They will be generated using random variates
    // generated by 'engine'.
    protected_engine<std::mt19937> engine(std::time(0));

    ParallelMinimizer minimizer(std::forward<FUNC&&>(func),
                                solutions,
                                starting_point_volume,
                                engine,
                                max_attempts);

    for (int i = 0; i != num_starting_points; ++i) {
      // We have to give a callable with no arguments to tasks.run, so we need
      // a lambda expression that captures all the arguments to be used
      // for the call to minimizer.
      tasks.run([minimizer]() { minimizer(); });
    }
    // Wait for all the tasks in the group to finish.
    tasks.wait();

    return {solutions.solutions(), minimizer.num_attempts()};
  }

  template <typename FUNC, typename REGION>
  minimization_results
  find_global_minimum_fixed(FUNC&& func,
                            int num_starting_points,
                            REGION const& starting_point_volume,
                            double tolerance,
                            long max_attempts)
  {
    using tt = typename callable_traits<FUNC>::template arg_t<0>;
    int const N = sizeof(tt) / sizeof(double);
    shared_result solutions(tolerance, num_starting_points);
    // The task group is what we use to schedule tasks to run.
    oneapi::tbb::task_group tasks;

    // All our starting points will be generated within the region
    // 'starting_point_volume'. They will be generated using random variates
    // generated by 'engine'.
    protected_engine<std::mt19937> engine(std::time(0));
    ParallelMinimizer minimizer(
      std::forward<FUNC&&>(func), solutions, starting_point_volume, engine);

    for (int i = 0; i != num_starting_points; ++i) {
      // We have to give a callable with no arguments to tasks.run, so we need
      // a lambda expression that captures all the arguments to be used
      // for the call to minimizer.
      tasks.run([minimizer]() { minimizer(); });
    }
    // Wait for all the tasks in the group to finish.
    tasks.wait();
    return {solutions.solutions(), minimizer.num_attempts()};
  }
}
#endif

#include "geometry.hh"
#include "minimizers.hh"
#include "protected_engine.hh"
#include "rosenbrock.hh"
#include "shared_result.hh"
#include "solution.hh"

#include "tbb/task_arena.h" // for default_concurrency()
#include "tbb/task_group.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

// This is a simple wrapper to adapt the pfc::rastrigin function to the
// interface expected by dlib.
inline double
rosenbrock_dlib_wrapper(pfc::column_vector const& x)
{
  using namespace std; // to allow std::begin to be found
  return pfc::vec_rosenbrock({begin(x), end(x)});
}

// This is the function that does all the minimization work. We use this to
// provide a scope that ensures that the object 'solutions' has a longer
// lifetime than the task_group 'tasks'.
// It is a blocking function that schedules parallel work, and waits until that
// work is done before returning.
void
do_all_work(long ndim, pfc::shared_result& solutions, int num_starting_points)
{
  // The task group is what we use to schedule tasks to run.
  oneapi::tbb::task_group tasks;

  // All our starting points will be generated within the region
  // 'starting_point_volume'. They will be generated using random variates
  // generated by 'engine'.
  pfc::region const starting_point_volume =
    pfc::make_box_in_n_dim(ndim, -10.0, 10.0);
  pfc::protected_engine<std::mt19937> engine;

  for (int i = 0; i != num_starting_points; ++i) {

    pfc::ParallelMinimizer minimizer(rosenbrock_dlib_wrapper,
                                     solutions,
                                     starting_point_volume,
                                     engine,
                                     tasks);

    // We have to give a callable with no arguments to tasks.run, so we need
    // a lambda expression that captures all the arguments to be used
    // for the call to minimizer.
    tasks.run([minimizer]() { minimizer(minimizer); });
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

  int const num_starting_points = oneapi::tbb::info::default_concurrency();

  // Create shared state for answer.
  // We're done when we have found a minimum with a value < 1.0e-6.
  pfc::shared_result solutions(1.0e-6, num_starting_points);

  do_all_work(ndim, solutions, num_starting_points);
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
  std::cout << "min\tdist\tnsteps\n";

  for (auto const& result : results) {
    std::cout << result << '\n';
  }
}
add_library(profiled_fc_cpu minuit2_support.cc rosenbrock.cc)
target_include_directories(profiled_fc_cpu PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(profiled_fc_cpu PRIVATE Minuit2::Minuit2)

add_executable(rosenbrock_test rosenbrock.test.cc)
target_include_directories(rosenbrock_test PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(rosenbrock_test PRIVATE Catch2::Catch2WithMain profiled_fc_cpu Boost::stacktrace_basic)
add_test(rosenbrock_test rosenbrock_test)

add_executable(minuit2_example minuit2_example.cc)
target_include_directories(minuit2_example PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(minuit2_example PRIVATE Minuit2::Minuit2 profiled_fc_cpu)

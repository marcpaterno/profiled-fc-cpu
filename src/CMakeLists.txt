add_executable(rosenbrock_test rosenbrock.test.cc)
target_link_libraries(rosenbrock_test PRIVATE Catch2::Catch2WithMain)
add_test(rosenbrock_test rosenbrock_test)

add_executable(minuit2_example minuit2_example.cc)
target_link_libraries(minuit2_example PRIVATE Minuit2::Minuit2)

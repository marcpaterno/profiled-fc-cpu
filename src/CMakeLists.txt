add_library(profiled_fc_cpu rosenbrock.cc rastrigin.cc
                            solution.cc shared_result.cc)
target_include_directories(
  profiled_fc_cpu PUBLIC ${PROJECT_SOURCE_DIR}/src
                         ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(
  profiled_fc_cpu
  PUBLIC fmt::fmt dlib::dlib
  PRIVATE ${CMAKE_DL_LIBS} TBB::tbb)

add_executable(optimization_ex optimization_ex.cc)
target_link_libraries(optimization_ex PRIVATE profiled_fc_cpu)

add_executable(dlib_rastrigin_example dlib_rastrigin_example.cc)
target_link_libraries(dlib_rastrigin_example PRIVATE profiled_fc_cpu)

add_executable(dlib_parallel_rastrigin_example
               dlib_parallel_rastrigin_example.cc)
target_include_directories(dlib_parallel_rastrigin_example
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(dlib_parallel_rastrigin_example
                      PRIVATE profiled_fc_cpu TBB::tbb fmt::fmt)

add_executable(dlib_parallel_rastrigin_example_5d
               dlib_parallel_rastrigin_example_5d.cc)
target_include_directories(dlib_parallel_rastrigin_example_5d
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(dlib_parallel_rastrigin_example_5d
                      PRIVATE profiled_fc_cpu TBB::tbb fmt::fmt)

add_executable(dlib_parallel_rosenbrock_example
               dlib_parallel_rosenbrock_example.cc)
target_include_directories(dlib_parallel_rosenbrock_example
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(dlib_parallel_rosenbrock_example
                      PRIVATE profiled_fc_cpu TBB::tbb fmt::fmt)

add_executable(geometry_test geometry.test.cc)
target_include_directories(geometry_test PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(geometry_test PRIVATE Catch2::Catch2WithMain
                                            profiled_fc_cpu)
add_test(geometry_test geometry_test)

add_executable(tbb_example tbb_example.cc)
target_link_libraries(tbb_example PRIVATE TBB::tbb)

add_executable(shared_result_test shared_result.test.cc)
target_include_directories(shared_result_test PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(shared_result_test PRIVATE Catch2::Catch2WithMain
                                                 profiled_fc_cpu)
add_test(shared_result_test shared_result_test)

add_executable(dlib_serial_rosenbrock_example dlib_serial_rosenbrock_example.cc)
target_link_libraries(dlib_serial_rosenbrock_example PRIVATE profiled_fc_cpu)

add_executable(dlib_serial_helical_valley_example
               dlib_serial_helical_valley_example.cc)
target_link_libraries(dlib_serial_helical_valley_example
                      PRIVATE profiled_fc_cpu)

add_executable(dlib_parallel_helical_valley_example
               dlib_parallel_helical_valley_example.cc)
target_include_directories(dlib_parallel_helical_valley_example
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/include)
target_link_libraries(dlib_parallel_helical_valley_example
                      PRIVATE profiled_fc_cpu TBB::tbb fmt::fmt)

cmake_minimum_required(VERSION 3.17)
project(SBCMin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(cryptominisat5 REQUIRED)

add_library(SBCMin SHARED machines/machines.cpp machines/machines.h
        minimizers/driven_fsm_minimizer.cpp minimizers/driven_fsm_minimizer.h machines/builders.cpp machines/builders.h
        minimizers/compat_matrix.cpp minimizers/compat_matrix.h minimizers/ofa_minimizer.cpp minimizers/ofa_minimizer.h
        minimizers/OFAMinimizers/basic_incremental.cpp minimizers/OFAMinimizers/basic_incremental.h
        minimizers/hopcroft.h minimizers/OFAMinimizers/basic_non_incremental.cpp
        minimizers/OFAMinimizers/basic_non_incremental.h minimizers/hopcroft.cpp
        minimizers/OFAMinimizers/assumption_based.cpp minimizers/OFAMinimizers/assumption_based.h
        minimizers/OFAMinimizers/w_set_priority.cpp minimizers/OFAMinimizers/w_set_priority.h
        machines/serializer.cpp machines/serializer.h minimizers/monotone_range_searchers.h
        minimizers/OFAMinimizers/w_state_priority.cpp minimizers/OFAMinimizers/w_state_priority.h)

add_executable(run_benchmark run_benchmark.cpp)
add_executable(run_benchmark_KN run_benchmark_KN.cpp)

target_include_directories(SBCMin PRIVATE ${CRYPTOMINISAT5_INCLUDE_DIRS})

target_link_libraries(SBCMin PUBLIC ${CRYPTOMINISAT5_LIBRARIES})

add_executable(check check.cpp test/experiment.cpp test/experiment.h)

target_link_libraries(check PUBLIC SBCMin)

target_link_libraries(run_benchmark PUBLIC SBCMin)

target_link_libraries(run_benchmark_KN PUBLIC SBCMin)
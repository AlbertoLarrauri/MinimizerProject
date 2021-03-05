cmake_minimum_required(VERSION 3.17)
project(PMIN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(cryptominisat5 REQUIRED)

add_executable(PMIN main.cpp machines/machines.cpp machines/machines.h minimizers/driven_fsm_minimizer.cpp minimizers/driven_fsm_minimizer.h machines/builders.cpp machines/builders.h minimizers/compat_matrix.cpp minimizers/compat_matrix.h minimizers/ofa_minimizer.cpp minimizers/ofa_minimizer.h minimizers/OFAMinimizers/basic_incremental.cpp minimizers/OFAMinimizers/basic_incremental.h minimizers/hopcroft.h minimizers/OFAMinimizers/basic_non_incremental.cpp minimizers/OFAMinimizers/basic_non_incremental.h minimizers/hopcroft.cpp minimizers/OFAMinimizers/assumption_based.cpp minimizers/OFAMinimizers/assumption_based.h minimizers/OFAMinimizers/w_symmetry_breaking.cpp minimizers/OFAMinimizers/w_symmetry_breaking.h)

target_include_directories(PMIN PRIVATE ${CRYPTOMINISAT5_INCLUDE_DIRS})

target_link_libraries(PMIN PUBLIC ${CRYPTOMINISAT5_LIBRARIES})
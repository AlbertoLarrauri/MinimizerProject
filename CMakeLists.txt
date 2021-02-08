cmake_minimum_required(VERSION 3.17)
project(PMIN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
find_package(cryptominisat5 REQUIRED)

add_executable(PMIN main.cpp fsms.cpp fsms.h driven_fsm_minimizer.cpp driven_fsm_minimizer.h)

target_include_directories(PMIN PRIVATE ${CRYPTOMINISAT5_INCLUDE_DIRS})

target_link_libraries(PMIN PUBLIC ${CRYPTOMINISAT5_LIBRARIES})
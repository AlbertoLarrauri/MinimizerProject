cmake_minimum_required(VERSION 3.17)
project(PMIN)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")
LINK_DIRECTORIES(/usr/local/lib)


add_executable(PMIN main.cpp fsms.cpp fsms.h driven_fsm_minimizer.cpp driven_fsm_minimizer.h)

target_link_libraries(PMIN cryptominisat5)
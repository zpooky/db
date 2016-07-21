cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wextra -Wall")

set(BOOST_ROOT /local/tools/boost_1_61_0)
set(BOOST_INCLUDE_DIR ${BOOST_ROOT}) # headers
set(BOOST_LIBRARY_DIR ${BOOST_ROOT}/stage/lib)# linked library(so)

add_subdirectory(main)

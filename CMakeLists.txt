cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wextra -Wall")

set(BOOST_ROOT /local/tools/boost_1_61_0)
set(BOOST_INCLUDEDIR /local/tools/boost_1_61_0) # headers
set(BOOST_INCLUDE_DIR /local/tools/boost_1_61_0) # headers
set(BOOST_LIBRARY_DIR /local/tools/boost_1_61_0/stage/lib)# linked library(so)

add_subdirectory(main)

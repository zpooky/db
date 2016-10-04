cmake_minimum_required(VERSION 2.8)

set(COLLECTION_LIB_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET collection_lib_test)

    message("collection_lib_test is already defined")

else ()
    # GTest
    add_subdirectory(googletest/googletest)
    include_directories(${gtest_SOURCE_DIR}/include)
    #
    include_directories(${COLLECTION_LIB_INCLUDE_DIR})
    link_directories(${COLLECTION_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES ListTest.cpp BitsetTest.cpp QueueTest.cpp FifoTest.cpp BaseTest.cpp BinaryTree.cpp ForwardTest.cpp SharedConcurrentPointerTest.cpp)
    ##############
    # Unit Tests
    ##############
    add_executable(collection_lib_test ${SOURCE_FILES})

    # Standard linking to gtest stuff.
    target_link_libraries(collection_lib_test gtest gtest_main)
    # Extra linking for the project.
    target_link_libraries(collection_lib_test collection_lib)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.
    add_test(collection_lib_test collection_lib_test)
endif ()

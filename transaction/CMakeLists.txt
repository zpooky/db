cmake_minimum_required(VERSION 2.8)

set(TRANSACTION_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET transaction_lib)

    message("transaction_lib is already defined")

else ()
    #
    add_subdirectory(../shared shared_lib)
    include_directories(shared)
    link_directories(${SHARED_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES transaction.h)

    add_library(transaction_lib STATIC ${SOURCE_FILES})
    set_target_properties(transaction_lib PROPERTIES LINKER_LANGUAGE CXX)
    #
    target_link_libraries(transaction_lib shared_lib)
endif ()
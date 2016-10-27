cmake_minimum_required(VERSION 2.8)

set(JOURNAL_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET journal_lib)

    message("journal_lib is already defined")

else ()
    #
    add_subdirectory(../shared shared_lib)
    include_directories(shared)
    link_directories(${SHARED_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES Journal.h)

    add_library(journal_lib STATIC ${SOURCE_FILES})
    set_target_properties(journal_lib PROPERTIES LINKER_LANGUAGE CXX)
    #
    target_link_libraries(journal_lib shared_lib)
endif ()
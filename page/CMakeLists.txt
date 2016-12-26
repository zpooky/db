cmake_minimum_required(VERSION 2.8)

set(PAGE_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET page_lib)

    message("page_lib is already defined")

else ()
    #
    # dependencies
    add_subdirectory(../shared shared_lib)
    include_directories(../shared)
    link_directories(${SHARED_LIB_INCLUDE_DIR})
    #
    add_subdirectory(../config config_lib)
    include_directories(../config)
    link_directories(${CONFIG_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES Transient.cpp
                     Transient.h
                     format/Format.cpp
                     format/Format.h
        )

    add_library(page_lib STATIC ${SOURCE_FILES})
    set_target_properties(page_lib PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(page_lib shared_lib)
    target_link_libraries(page_lib config_lib)
endif ()

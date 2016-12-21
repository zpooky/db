cmake_minimum_required(VERSION 3.2)

set(CONFIG_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET config_lib)

    message("config_lib is already defined")

else ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES )

    add_library(config_lib STATIC ${SOURCE_FILES})
    set_target_properties(config_lib PROPERTIES LINKER_LANGUAGE CXX)
endif ()

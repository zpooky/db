cmake_minimum_required(VERSION 2.8)

set(FS_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET fs_lib)

    message("fs_lib is already defined")

else ()
    #
    # dependencies
    add_subdirectory(../shared shared_lib)
    include_directories(../shared)
    link_directories(${SHARED_LIB_INCLUDE_DIR})
    #
    add_subdirectory(../collection collection_lib)
    include_directories(../collection)
    link_directories(${COLLECTION_LIB_INCLUDE_DIR})
    #
    add_subdirectory(../config config_lib)
    include_directories(../config)
    link_directories(${CONFIG_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES Line.cpp
                     FileWriter.cpp
                     FileReader.cpp
                     )

    add_library(fs_lib STATIC ${SOURCE_FILES})
    set_target_properties(fs_lib PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(fs_lib shared_lib)
    target_link_libraries(fs_lib config_lib)
     
    add_subdirectory(test)
endif ()

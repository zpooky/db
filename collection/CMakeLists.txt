cmake_minimum_required(VERSION 2.8)

set(COLLECTION_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET collection_lib)

    message("collection_lib is already defined")

else ()
    # dependencies
    add_subdirectory(../shared shared_lib)
    include_directories(shared)
    link_directories(${SHARED_LIB_INCLUDE_DIR})
    #Test

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES MPSCQueue.h List.h CBitset.h LIFO.h Barrier.cpp Barrier.h)

    add_library(collection_lib STATIC ${SOURCE_FILES})
    set_target_properties(collection_lib PROPERTIES LINKER_LANGUAGE CXX)
    #
    target_link_libraries(collection_lib shared_lib)
endif ()

add_subdirectory(test)

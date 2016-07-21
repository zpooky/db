cmake_minimum_required(VERSION 2.8)

set(SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)
if (TARGET shared_lib)

    message("shared_lib is already defined")

else ()
    # boost
    include_directories(${BOOST_INCLUDE_DIR}) #include the headers
    link_directories(${BOOST_LIBRARY_DIR})
    find_package(Boost COMPONENTS filesystem REQUIRED)
    #
    add_subdirectory(../config config_lib)
    include_directories(../config)
    link_directories(${CONFIG_LIB_INCLUDE_DIR})
    #

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES fs.h Buffer.h Assertions.h vfs.cpp vfs.h entities.cpp entities.h hash.cpp hash.h TableMeta.h Maybe.h conversions.h)

    add_library(shared_lib STATIC ${SOURCE_FILES})
    set_target_properties(shared_lib PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(shared_lib ${Boost_LIBRARIES})
endif ()
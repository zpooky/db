cmake_minimum_required(VERSION 2.8)

set(FS_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET segment_lib)

    message("segment_lib is already defined")

else ()
    #
    find_package (Threads)
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
    add_subdirectory(../fs fs_lib)
    include_directories(../fs)
    link_directories(${FS_LIB_INCLUDE_DIR})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    set(SOURCE_FILES SegmentReservations.h
                     Segment.cpp
                     Reservations.h
                     Segments.h
                     PresentSet.h
                     ReservationSet.cpp
                     ReservationSet.h
                     Context.h)

    add_library(segment_lib STATIC ${SOURCE_FILES})
    set_target_properties(segment_lib PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(segment_lib shared_lib)
    target_link_libraries(segment_lib config_lib)
    target_link_libraries(segment_lib fs_lib)
    target_link_libraries(segment_lib Threads::Threads)
endif ()

#add_subdirectory(test)

cmake_minimum_required(VERSION 3.2)
project(db)

#journal
add_subdirectory(../journal journal_lib)
include_directories(journal)
link_directories(${JOURNAL_LIB_INCLUDE_DIR})
#config
add_subdirectory(../config config_lib)
include_directories(config)
link_directories(${CONFIG_LIB_INCLUDE_DIR})
#fs
add_subdirectory(../fs fs_lib)
include_directories(fs)
link_directories(${FS_LIB_INCLUDE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp TestTable.h TableFacade.cpp TableFacade.h)
add_executable(db ${SOURCE_FILES})
target_link_libraries(db journal_lib)
target_link_libraries(db config_lib)
target_link_libraries(db fs_lib)
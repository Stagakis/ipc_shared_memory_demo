cmake_minimum_required(VERSION 3.10)

project(ipc_demo)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ipc_demo demo.cpp)

target_link_libraries(ipc_demo PUBLIC -lpthread)
target_link_libraries(ipc_demo PUBLIC -lrt)
target_link_libraries(ipc_demo PUBLIC ${OpenCV_LIBS} )
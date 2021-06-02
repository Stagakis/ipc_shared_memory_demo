cmake_minimum_required(VERSION 3.10)

project(ipc_demo)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ipc_demo demo.cpp demo.h triple_buffer_ipc.cpp triple_buffer_ipc.h)

target_link_libraries(ipc_demo PUBLIC -lpthread)
target_link_libraries(ipc_demo PUBLIC -lrt)
target_link_libraries(ipc_demo PUBLIC ${OpenCV_LIBS} )


project(ipc_demo_no_atomic)

add_executable(ipc_demo_no_atomic demo_noatomic.cpp)

target_link_libraries(ipc_demo_no_atomic PUBLIC -lpthread)
target_link_libraries(ipc_demo_no_atomic PUBLIC -lrt)
target_link_libraries(ipc_demo_no_atomic PUBLIC ${OpenCV_LIBS} )


project(ipc_library VERSION 1.0.0 DESCRIPTION "my_shared_ipc description")
add_library(ipc_library SHARED
        triple_buffer_ipc.cpp
        triple_buffer_ipc.h)
target_link_libraries(ipc_library PUBLIC -lpthread)
target_link_libraries(ipc_library PUBLIC -lrt)
target_link_libraries(ipc_library PUBLIC ${OpenCV_LIBS} )
set_target_properties(ipc_library PROPERTIES PUBLIC_HEADER demo.h)
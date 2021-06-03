cmake_minimum_required(VERSION 3.12)


set(CMAKE_BUILD_TYPE Debug)
project(ipc_demo)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ipc_demo demo.cpp demo.h) # tbipc.cpp tbipc.h)

target_link_libraries(ipc_demo PUBLIC -lpthread)
target_link_libraries(ipc_demo PUBLIC -ltbipc)
target_link_libraries(ipc_demo PUBLIC -lrt)
target_link_libraries(ipc_demo PUBLIC ${OpenCV_LIBS} )

project(tbipc)
add_library(tbipc SHARED
        tbipc/tbipc.cpp
        tbipc/tbipc.h)
target_link_libraries(tbipc PUBLIC -lpthread)
target_link_libraries(tbipc PUBLIC -lrt)

install(TARGETS tbipc DESTINATION lib)
install(FILES tbipc/tbipc.h DESTINATION include)
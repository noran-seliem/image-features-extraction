cmake_minimum_required(VERSION 2.8)
project( SNAKES )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_CXX_STANDARD 17)
add_executable( SNAKES main.cpp)
target_link_libraries( SNAKES ${OpenCV_LIBS} )
target_include_directories(SNAKES PRIVATE ${YOUR_DIRECTORY})

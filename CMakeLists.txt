cmake_minimum_required(VERSION 3.9)

project(BinaryArithmeticCodec)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
set(SOURCE_FILES ${SRC_FILES})

find_package(OpenCV REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(codec ${SOURCE_FILES})

target_link_libraries(codec ${OpenCV_LIBS} )
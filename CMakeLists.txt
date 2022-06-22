cmake_minimum_required(VERSION 3.22)

project(sio-bridge)

set(EXEC sio-bridge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_CXX_STANDARD 14)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

file(GLOB SOURCES src/*.cpp)

add_executable(${EXEC} ${SOURCES})
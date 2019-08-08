cmake_minimum_required(VERSION 3.8.4)
project(box LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib/include)

add_subdirectory(test)

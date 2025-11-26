cmake_minimum_required(VERSION 4.0)
project(unit_hpp)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(unit_hpp test.cpp Unit.hpp Vector.hpp Matrix.hpp
        Rect.hpp)

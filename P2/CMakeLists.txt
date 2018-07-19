cmake_minimum_required(VERSION 3.9)
project(P2_)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(P2_
        P2/Complex.cc
        P2/Complex.h
        P2/fft2d.cc
        P2/InputImage.cc
        P2/InputImage.h)

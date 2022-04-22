cmake_minimum_required(VERSION 3.7.2)
project(BS_Praktikum_22)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(.idea)

add_executable(BS_Praktikum_22
        main.c keyValStore.c keyValStore.h sub.c sub.h main.h)

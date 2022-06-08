cmake_minimum_required(VERSION 3.7.2)
project(BS_Praktikum_22)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(.idea)

add_executable(BS_Praktikum_22
        main.c
        main.h
        keyValStore.c
        keyValStore.h
        server.c
        server.h
        splitCommand.c
        splitCommand.h)

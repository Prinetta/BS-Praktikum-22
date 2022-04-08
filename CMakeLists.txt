cmake_minimum_required(VERSION 3.10)
project(BS_Praktikum_22)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(.idea)

add_executable(BS_Praktikum_22
        .idea/.gitignore
        .idea/BS-Praktikum-22.iml
        .idea/deployment.xml
        .idea/modules.xml
        .idea/vcs.xml
        .idea/workspace.xml
        main.c keyValStore.c keyValStore.h sub.c sub.h main.h)

cmake_minimum_required(VERSION 3.21)
project(CT)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(CT
        lexical_analysis.cpp
        lexical_analysis.h
        main.cpp
        semantic_analysis.cpp
        semantic_analysis.h
        symbol_table.cpp
        symbol_table.h
        types_analysis.cpp
        instruction.h
        instruction.cpp
        vmutils.cpp
        vm.cpp)

add_executable(VM vm.cpp vm.h instruction.cpp instruction.h vmutils.cpp)
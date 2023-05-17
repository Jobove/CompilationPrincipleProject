cmake_minimum_required(VERSION 3.25)
project(CompilationPrinciple)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIR_SRCS)

include_directories(./ ./Lib)

add_subdirectory(Lib)

add_executable(CompilationPrinciple ${DIR_SRCS})

target_link_libraries(CompilationPrinciple Lib)

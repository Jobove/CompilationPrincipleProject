cmake_minimum_required(VERSION 3.25)
project(CompilationPrinciple)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpclmul -mavx2")

aux_source_directory(. DIR_SRCS)

include_directories(./ ./Lex ./Grammar)
include_directories(D:/nlohmann/include)

add_subdirectory(Lex)
add_subdirectory(Grammar)

add_executable(CompilationPrinciple ${DIR_SRCS})

target_link_libraries(CompilationPrinciple Lex Grammar)

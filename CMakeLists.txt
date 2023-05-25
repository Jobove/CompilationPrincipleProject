cmake_minimum_required(VERSION 3.25)
project(CompilationPrinciple)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. DIR_SRCS)

find_package(Boost COMPONENTS regex REQUIRED)

include_directories(
        ./
        ./Lex
        ./Grammar
        ${Boost_INCLUDE_DIRS}
        C:/nlohmann/include
)

add_subdirectory(Lex)
add_subdirectory(Grammar)
add_subdirectory(Config)

add_executable(CompilationPrinciple ${DIR_SRCS})

target_link_libraries(CompilationPrinciple
        Lex
        Grammar
        ${BOOST_LIBRARIES}
        )

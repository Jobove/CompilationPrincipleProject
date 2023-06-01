cmake_minimum_required(VERSION 3.25)
project(CompilationPrinciple VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/mingw_64")   # Qt的安装路径
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Close the warning for the new version boost

aux_source_directory(. DIR_SRCS)

find_package(Boost COMPONENTS REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

include_directories(
        ./Lex
        ./Grammar
        ${Boost_INCLUDE_DIRS}
        C:/nlohmann/include
)

set(PROJECT_SOURCES
        main.cpp
        widget.h
        widget.cpp
        widget.ui
        )

add_subdirectory(Lex)
add_subdirectory(Grammar)
add_subdirectory(Config)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CompilationPrinciple
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    if (ANDROID)
        add_library(CompilationPrinciple SHARED
                ${PROJECT_SOURCES}
                )
    else ()
        add_executable(CompilationPrinciple
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

target_link_libraries(CompilationPrinciple
        PRIVATE Lex
        PRIVATE Grammar
        PRIVATE ${BOOST_LIBRARIES}
        PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        PRIVATE Qt${QT_VERSION_MAJOR}::Sql
        )

set_target_properties(CompilationPrinciple PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

install(TARGETS CompilationPrinciple
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CompilationPrinciple)
endif ()
cmake_minimum_required(VERSION 3.4.1)

project(reindeer2d)

set(USE_GLES false CACHE BOOL "use gles")
set(USE_EGL false CACHE BOOL "use egl")

add_library(reindeer2d
    STATIC
    src/Canvas.cpp
    src/Container.cpp
)

target_include_directories(reindeer2d
    PUBLIC
    include
)

set_target_properties(reindeer2d
    PROPERTIES COMPILE_FLAGS
    "-Wno-c++11-extensions"
)

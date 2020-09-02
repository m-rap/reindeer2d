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

set(RDR_CFLAGS "-Wno-writable-strings -Wno-c++11-extensions")

if (USE_EGL)
    set(RDR_CFLAGS "${RDR_CFLAGS} -DUSE_EGL")
endif()

if (USE_GLES)
    set(RDR_CFLAGS "${RDR_CFLAGS} -DUSE_GLES")
endif()
    

set_target_properties(reindeer2d
    PROPERTIES COMPILE_FLAGS
    "${RDR_CFLAGS}"
)

if (ANDROID_NDK)
    add_library(reindeer2d-example
        SHARED
        examples/android-example.cpp
    )
    target_link_libraries(reindeer2d-example
        reindeer2d
        android
        EGL
        GLESv1_CM
        log
    )
    
    set_target_properties(reindeer2d-example
        PROPERTIES COMPILE_FLAGS "${RDR_CFLAGS}"
    )

    set(EXAMPLE_LINK_FLAGS "-u ANativeActivity_onCreate")

    set_target_properties(reindeer2d-example
        PROPERTIES LINK_FLAGS "${EXAMPLE_LINK_FLAGS}"
    )
endif()

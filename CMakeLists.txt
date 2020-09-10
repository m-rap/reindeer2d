cmake_minimum_required(VERSION 3.4.1)

project(reindeer2d)

set(USE_GLES false CACHE BOOL "use gles")
set(USE_EGL false CACHE BOOL "use egl")

if (ANDROID_NDK)
    set(USE_GLES true)
    set(USE_EGL true)
    set(ANDROID_NDK_SRC ${ANDROID_NDK}/sources)
endif()

set(RDR_SRC
    src/Canvas.c
    src/Container.c
)

set(RDR_CFLAGS "-Wno-writable-strings -Wno-c++11-extensions -fPIC")

if (USE_EGL)
    set(RDR_CFLAGS "${RDR_CFLAGS} -DUSE_EGL")
    set(RDR_SRC
        ${RDR_SRC}
        src/EglContainer.c
    )
endif()

if (USE_GLES)
    set(RDR_CFLAGS "${RDR_CFLAGS} -DUSE_GLES")
endif()

add_library(reindeer2d
    STATIC
    ${RDR_SRC}
)

target_include_directories(reindeer2d
    PUBLIC
    include
)
    

set_target_properties(reindeer2d
    PROPERTIES COMPILE_FLAGS
    "${RDR_CFLAGS}"
)

if (ANDROID_NDK)
    add_library(reindeer2d-example
        SHARED
        #examples/android-example.cpp
        examples/android-example.c
        ${ANDROID_NDK_SRC}/android/native_app_glue/android_native_app_glue.c
    )
    target_include_directories(reindeer2d-example PUBLIC
        ${ANDROID_NDK_SRC}/android/native_app_glue)
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

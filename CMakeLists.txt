cmake_minimum_required(VERSION 4.2)

if (UNIX)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/deps/RE-UE4SS/cmake/toolchains/xwin-clang-cl-toolchain.cmake)
endif ()

project(MyMod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ignore Warnings
add_compile_options("-w")

add_subdirectory(deps EXCLUDE_FROM_ALL)

set(TARGET MyMod)

add_library(${TARGET} SHARED
     src/Mod.cpp
)

target_compile_definitions(${TARGET}
        PRIVATE
        NOMINMAX
)

target_link_libraries(${TARGET}
        PRIVATE
	MyModDeps
)

set_target_properties(${TARGET} PROPERTIES
        OUTPUT_NAME "main"
)

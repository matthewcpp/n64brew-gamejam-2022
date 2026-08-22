cmake_minimum_required(VERSION 3.16)

include(lib/framework64/cmake/configure_platform.cmake REQUIRED)

if (FW64_PLATFORM_N64)
    project(dead_ritual C)
else()
    project(dead_ritual)
endif()

set(Fw64_BUILD_TESTS OFF)
set(FW64_BUILD_EXAMPLES OFF)
set(FW64_BUILD_TOOLS OFF)

add_subdirectory(lib/framework64)
add_subdirectory(src)
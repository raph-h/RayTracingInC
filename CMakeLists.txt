﻿# CMakeList.txt : CMake project for RayTracing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

add_compile_options("/Zc:__cplusplus")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RayTracing")

# Add source to this project's executable.
add_executable (RayTracing "RayTracing.cpp" "RayTracing.hpp" "vec3.hpp" "colour.hpp" "ray.hpp" "objects/hittable.hpp" "objects/sphere.hpp" "objects/hittable_list.hpp"  "interval.hpp" "camera.hpp" "material.hpp" "aabb.hpp" "external/stb_image.c" "external/stb_image_write.c" "onb.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RayTracing PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
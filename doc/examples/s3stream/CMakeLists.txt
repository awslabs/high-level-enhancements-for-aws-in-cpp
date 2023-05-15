# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

# -- Preamble --
project("awslabs_enhanced_cpp_examples"
        LANGUAGES CXX
        VERSION "0.1.0"
        DESCRIPTION "High-level enhancements for AWS in C++ examples."
        )

# -- Project wide setup --
# Setting C++ minimum requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setting build to hide symbols in targets by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Preventing writes to package registry by default
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY YES)

# Validating config type and setting default if needed
get_property(is_multi_conf_build GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT is_multi_conf_build)
    set(allowed_build_types Debug Release RelWithDebInfo MinSizeRel)
    # cmake-gui helper
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowed_build_types}")
    if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
    elseif (NOT CMAKE_BUILD_TYPE IN_LIST allowed_build_types)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif ()
endif ()

# -- Dependencies --
find_package(awslabs_enhanced_cpp)
find_package(tl-expected)
find_package(AWSSDK REQUIRED COMPONENTS s3)

# -- Targets --
# S3 read object example
add_executable(read_object read_object.cpp)
target_link_libraries(read_object
        PRIVATE
        awslabs_enhanced_cpp::headers
        aws-cpp-sdk-s3 aws-cpp-sdk-core
        )
add_executable(write_object write_object.cpp)
target_link_libraries(write_object
        PRIVATE
        awslabs_enhanced_cpp::headers
        aws-cpp-sdk-s3 aws-cpp-sdk-core
        )
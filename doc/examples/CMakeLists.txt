# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

# -- Preamble --
project("awslabs_enhanced_cpp_examples"
        LANGUAGES CXX
        VERSION "0.1.0"
        DESCRIPTION "High-level enhanced for AWS in C++ examples."
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
find_package(aws-lambda-runtime)
find_package(awslabs_enhanced_cpp)
find_package(tl-expected)
find_package(AWSSDK REQUIRED COMPONENTS s3 lambda)

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    alpaca
    GIT_REPOSITORY https://github.com/p-ranav/alpaca.git
)
FetchContent_MakeAvailable(alpaca)

# Add S3 examples
add_subdirectory(s3stream)

# -- Targets --
# Lambda addition example
add_executable(lambda_add lambda_add_example.cpp)
target_link_libraries(lambda_add
        PRIVATE
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        aws-cpp-sdk-lambda
        alpaca
        )

add_executable(lambda_add_fn lambda_add_fn.cpp)
target_link_libraries(lambda_add_fn
        PRIVATE
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        alpaca
        )
# Lambda rr example
#add_executable(lambda_rr lambda_rr_cloud_main.cpp)
#target_link_libraries(lambda_rr
#        PRIVATE
#        awslabs_enhanced_cpp::headers
#        aws-cpp-sdk-core
#        )

set(CPACK_PROJECT_NAME lambda_add)
set(CPACK_PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
include(CPack)

aws_lambda_package_target(lambda_add_fn)

add_executable(exp_mean_fn exp_mean.cpp)
target_link_libraries(exp_mean_fn
        PRIVATE
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        alpaca
        )
target_compile_definitions(exp_mean_fn  PRIVATE AWS_LAMBDA)
aws_lambda_package_target(exp_mean_fn)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.0.0  
)
FetchContent_MakeAvailable(cxxopts)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(central_limit_theorem central_limit_theorem.cpp exp_mean.cpp)
target_link_libraries(central_limit_theorem
        PRIVATE
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        alpaca
        aws-cpp-sdk-lambda
        fmt
        cxxopts
        Threads::Threads
        )

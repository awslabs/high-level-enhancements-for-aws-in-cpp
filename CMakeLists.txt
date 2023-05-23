# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13)

# -- Preamble --
string(CONCAT DESCRIPTION_STRING "The AWSLabs Enhanced libraries for C++ tight integration of "
        "AWS Services with C++ standard libraries and containers.")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Git QUIET) # Adding development helper tools as git_hash built when available.

include(project_version)
obtain_project_version(AWSLABS_ENHANCED_CPP_PROJECT_VERSION awslabs-enhanced-cpp_GIT_HASH)

project("awslabs_enhanced_cpp"
        LANGUAGES CXX
        VERSION ${AWSLABS_ENHANCED_CPP_PROJECT_VERSION}
        DESCRIPTION ${DESCRIPTION_STRING}
        )
include(CTest)
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

message(STATUS "Generating source file for introspection of version to ${CMAKE_PROJECT_VERSION} ")
configure_file(
        version.h.in
        include/version.h
        @ONLY
)

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

# Options definition
option(BUILD_TESTING "If enabled, the SDK will include tests in the build" OFF)

# -- Dependencies --
include(dependencies)

# -- main build target --
add_library(headers INTERFACE)
add_library(awslabs_enhanced_cpp::headers ALIAS headers)

target_include_directories(headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

target_compile_features(headers INTERFACE cxx_std_20)
target_link_libraries(headers INTERFACE tl::expected aws-cpp-sdk-core)

# -- Tests, doc and packaging if running this as top project --
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif ()
    add_subdirectory(packaging)
endif ()

include(GNUInstallDirs)
set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})

install(TARGETS headers EXPORT awslabs_enhanced_cpp)

install(EXPORT awslabs_enhanced_cpp
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/awslabs_enhanced_cpp"
        NAMESPACE awslabs_enhanced_cpp::
        FILE awslabs_enhanced_cpp-config.cmake
        )

install(
        PROGRAMS tools/deploy_cpp_lambda_fn.sh
        DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME deploy_cpp_lambda_fn
)

install(
        PROGRAMS tools/cli_deploy_cpp_lambda.sh
        DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME cli_deploy_cpp_lambda
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        awslabs_enhanced_cpp-config-version.cmake
        COMPATIBILITY ExactVersion
)
install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/awslabs_enhanced_cpp-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/awslabs_enhanced_cpp
)
install(
        DIRECTORY doc
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/awslabs DESTINATION include)


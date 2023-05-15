# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

find_package(AWSSDK REQUIRED COMPONENTS s3 lambda)
find_package(aws-lambda-runtime)
find_package(tl-expected)

if (BUILD_TESTING)
    message(STATUS "Building tests")
    # Testing dependency
    find_package(GTest 1.11 REQUIRED)
    include(GoogleTest) # for gtest_discover_tests()
endif ()

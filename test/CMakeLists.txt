# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

message(STATUS "Preparing S3stream integration tests. Credentials in AWS to create testing objects required to run.")

add_executable(
        s3buf_integration_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s3buf.cpp
)
target_link_libraries(
        s3buf_integration_tests
        GTest::gtest_main
        awslabs_enhanced_cpp::headers
        ${AWSSDK_LINK_LIBRARIES}
        ${AWSSDK_PLATFORM_DEPS}
)
gtest_discover_tests(s3buf_integration_tests)

add_executable(
        os3stream_integration_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/test_os3stream.cpp
)
target_link_libraries(
        os3stream_integration_tests
        GTest::gtest_main
        awslabs_enhanced_cpp::headers
        ${AWSSDK_LINK_LIBRARIES}
        ${AWSSDK_PLATFORM_DEPS}
)
gtest_discover_tests(os3stream_integration_tests)

add_executable(
        is3stream_integration_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/test_is3stream.cpp
)
target_link_libraries(
        is3stream_integration_tests
        GTest::gtest_main
        awslabs_enhanced_cpp::headers
        ${AWSSDK_LINK_LIBRARIES}
        ${AWSSDK_PLATFORM_DEPS}
)
gtest_discover_tests(is3stream_integration_tests)
#[[
find_package(aws-lambda-runtime)
find_package(tl-expected)
add_executable(
        lambda_integration_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/test_lambda.cpp
)

target_link_libraries(
        lambda_integration_tests
        GTest::gtest_main
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        ${AWSSDK_LINK_LIBRARIES}
        ${AWSSDK_PLATFORM_DEPS}
)
]]
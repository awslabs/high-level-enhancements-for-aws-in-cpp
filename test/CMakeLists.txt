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

include(FetchContent)
FetchContent_Declare(
    alpaca
    GIT_REPOSITORY https://github.com/p-ranav/alpaca.git
)
FetchContent_MakeAvailable(alpaca)

add_executable(test_lambda_add_fn test_lambda_add_fn.cpp)
target_link_libraries(test_lambda_add_fn
        PRIVATE
        awslabs_enhanced_cpp::headers
        AWS::aws-lambda-runtime
        aws-cpp-sdk-core
        alpaca
        )

set(CPACK_PROJECT_NAME lambda_add)
set(CPACK_PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
include(CPack)

aws_lambda_package_target(test_lambda_add_fn)
add_custom_target(require_test_lambda_add_package ALL
        DEPENDS aws-lambda-package-test_lambda_add_fn)

add_test(
        NAME deploy_test_lambda_add_fn
        COMMAND ${CMAKE_COMMAND}
                -DLAMBDA_NAME=test_lambda_add_fn
                -DLAMBDA_EXECUTION_ROLE=${LAMBDA_EXECUTION_ROLE}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/redeploy_lambda.cmake
)

add_executable(test_lambda_add test_lambda_add.cpp)
target_link_libraries(test_lambda_add
        GTest::gtest_main
        awslabs_enhanced_cpp::headers
        aws-cpp-sdk-lambda)

gtest_discover_tests(test_lambda_add PROPERTIES DEPENDS deploy_test_lambda_add_fn)
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include "awslabs/enhanced/lambda_client.h"
#include "awslabs/enhanced/Aws.h"
#include "gtest/gtest.h"

namespace LambdaDecls {
    int add(int, int);
}

class lambdaIntegrationTest : public ::testing::Test {
protected:
  AwsLabs::Enhanced::AwsApi api;
  AwsLabs::Enhanced::EnhancedLambdaClient client;
};

TEST_F(lambdaIntegrationTest, TestCall) {
    auto add = BIND_AWS_LAMBDA(client, LambdaDecls::add, "test_lambda_add_fn");
    EXPECT_EQ(add(1, 3), 4);
}
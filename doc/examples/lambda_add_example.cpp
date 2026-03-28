/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "awslabs/enhanced/Aws.h"
#include "awslabs/enhanced/lambda_client.h"
#include <iostream>

using namespace AwsLabs::Enhanced;
AwsApi api;
AwsLogging awsLogging(Aws::Utils::Logging::LogLevel::Trace, "enhanced_lambda_");
EnhancedLambdaClient client;

namespace lambda_declarations {
int add(int, int);
}

auto add = BIND_AWS_LAMBDA(client, lambda_declarations::add, "add");

int main(int argc, char **argv) {
  std::cout << "1 + 3 is " << add(1, 3) << std::endl;
  return 0;
}

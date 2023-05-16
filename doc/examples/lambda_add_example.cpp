/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include "awslabs/enhanced/lambda_client.h"
#include "awslabs/enhanced/Aws.h"

using namespace AwsLabs::Enhanced;
AwsApi api;
AwsLogging awsLogging(Aws::Utils::Logging::LogLevel::Trace, "enhanced_lambda_");
EnhancedLambdaClient client;
namespace foo {
int add(int, int);
}
auto add = BIND_AWS_LAMBDA(client, foo::add, "Add");
//auto add = client.bind_lambda<int(int, int)>("add");

int main(int argc, char **argv)
{
    std::cout << "1 + 3 is " << add(1, 3) << std::endl;
    return 0;
}

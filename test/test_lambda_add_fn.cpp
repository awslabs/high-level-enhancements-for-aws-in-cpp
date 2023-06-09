/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "awslabs/enhanced/aws_lambda.h"
using namespace AwsLabs::Enhanced;

int add(int i, int j) { return i + j; }

AwsLabs::Enhanced::Handler handle(&add);

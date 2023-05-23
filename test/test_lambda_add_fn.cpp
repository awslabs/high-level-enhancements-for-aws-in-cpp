/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "awslabs/enhanced/aws_lambda.h"

int add(int i, int j) { return i + j; }

Handler handle(&add);

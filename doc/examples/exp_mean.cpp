/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <random>

double exp_mean(double lambda, int count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
 
    // if particles decay once per second on average,
    // how much time, in seconds, until the next one?
    std::exponential_distribution<> d(lambda);

    double total{};

    // Once compilers support ranges::fold_left, we'll use that
    for(int i{}; i < count; i++) {
        total += d(gen);
    }
    return total/count;
}

#ifdef AWS_LAMBDA
#include "awslabs/enhanced/aws_lambda.h"
Handler handle(&exp_mean);
#endif
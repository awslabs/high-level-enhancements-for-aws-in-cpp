/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "exp_mean.h"
#include <random>

double exp_mean(exp_parameters p)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    auto [lambda, samples] = p;
 
    // if particles decay once per second on average,
    // how much time, in seconds, until the next one?
    std::exponential_distribution<> d(lambda);

    double total{};

    // Once compilers support ranges::fold_left, we'll use that
    for(int i{}; i < samples; i++) {
        total += d(gen);
    }
    return total/samples;
}

#ifdef AWS_LAMBDA
#include "awslabs/enhanced/aws_lambda.h"
AwsLabs::Enhanced::Handler handle(&exp_mean);
#endif
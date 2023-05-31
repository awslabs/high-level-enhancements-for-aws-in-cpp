/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
struct exp_parameters {
    double lambda;
    unsigned samples;
};

double exp_mean(exp_parameters);
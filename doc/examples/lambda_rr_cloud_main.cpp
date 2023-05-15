/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "awslabs/enhanced/aws_lambda.h"
#include <aws/lambda-runtime/runtime.h>
using namespace aws::lambda_runtime;

// Example from https://github.com/awslabs/aws-lambda-cpp/README.md still works
invocation_response my_handler(invocation_request const& req)
{
    if (req.payload.length() > 42) {
        return invocation_response::failure("error message here"/*error_message*/,
                                            "error type here" /*error_type*/);
    }

    return invocation_response::success("json payload here" /*payload*/,
                                        "application/json" /*MIME type*/);
}

Handler handle(&my_handler);

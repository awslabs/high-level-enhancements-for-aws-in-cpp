#!/bin/sh
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

if [ $# -lt 3 ]; then
  echo "usage: $0 function_name iam_role lambda_packaged_binary"
  exit 1
fi
FUNCTION_NAME=$1
IAM_ROLE=$2
LAMBDA_PACK=$3

aws lambda delete-function --function-name ${FUNCTION_NAME}
aws lambda create-function --function-name ${FUNCTION_NAME} --role ${IAM_ROLE} --runtime provided --timeout 900 --memory-size 2038 --handler ${FUNCTION_NAME} --zip-file fileb://${LAMBDA_PACK}
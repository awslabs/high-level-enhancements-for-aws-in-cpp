#!/bin/sh
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# This script prepares a yaml file to deploy a lambda function and deploys it.

if [ $# -lt 5 ]; then
  echo "usage: $0 function_name account iam_role stack_name lambda_packaged_binary"
  exit 1
fi
FUNCTION_NAME=$1
ACCOUNT=$2
IAM_ROLE=$3
STACK=$4
LAMBDA_PACK=$5

sed "s/%%function_name%%/${FUNCTION_NAME}/g" $(dirname "${0}")/lambda_cfn_deploy.yaml-template |
  sed "s/%%iam_role%%/${IAM_ROLE}/g" |
  sed "s/%%account%%/${ACCOUNT}/g" \
    >"${FUNCTION_NAME}_lambda_cfn_deploy.yaml"

echo "Created ${FUNCTION_NAME}_lambda_cfn_deploy.yaml"

echo "Deploying to ${STACK} the lambda function ${FUNCTION_NAME} implemented by ${LAMBDA_PACK}"
rm -f "deployed_${STACK}_${FUNCTION_NAME}"
if sam deploy --resolve-s3 --template "${FUNCTION_NAME}_lambda_cfn_deploy.yaml" --stack-name "${STACK}"; then
  touch "deployed_${STACK}_${FUNCTION_NAME}"
else
  echo "Failed deploy"
  exit 2
fi

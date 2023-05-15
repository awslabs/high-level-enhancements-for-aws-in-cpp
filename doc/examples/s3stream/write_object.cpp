/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <awslabs/enhanced/s3stream.h>
#include <fstream>
#include <iostream>
#include <aws/core/Aws.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << "output_file" << std::endl;
    return 1; // Missing output file parameter
  }

  std::string test_content("Hello World!");

  std::cout << "Saving to a file" << std::endl;
  {
    std::ofstream ofs(argv[1]);
    ofs << test_content;
  }

  std::cout << "Saving an object to S3" << std::endl;
  Aws::SDKOptions options;
  InitAPI(options);
  {
    AwsLabs::Enhanced::os3stream os3s("us-east-1",
                            "test-bucket-s3stream",
                            argv[1]);
    os3s << test_content;
  }
  ShutdownAPI(options);
  return 0;
}
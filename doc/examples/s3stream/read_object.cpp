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
    std::cout << "usage: " << argv[0] << " input_file" << std::endl;
    return 1; // Missing output file parameter
  }

  std::cout << "Reading from a file" << std::endl;
  {
    std::ifstream ifs(argv[1]);
    ifs >> std::noskipws;
    std::copy(std::istream_iterator<char>(ifs),
              std::istream_iterator<char>(),
              std::ostream_iterator<char>(std::cout));
    ifs.close();
  }
  std::cout << std::endl;

  std::cout << "Reading from an object in S3" << std::endl;
  Aws::SDKOptions options;
  InitAPI(options);
  {
    AwsLabs::Enhanced::is3stream is3s("us-east-1",
                               "test-bucket-s3stream",
                               argv[1]);
    is3s >> std::noskipws;
    std::copy(std::istream_iterator<char>(is3s),
              std::istream_iterator<char>(),
              std::ostream_iterator<char>(std::cout));
    is3s.close();
  }
  ShutdownAPI(options);

  std::cout << std::endl;
  return 0;
}
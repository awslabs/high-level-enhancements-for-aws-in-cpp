/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <memory>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/LogSystemInterface.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>

namespace AwsLabs::Enhanced {
struct AwsApi {
  AwsApi(Aws::SDKOptions options = {}) : options(options) {
    static bool apiInitialized = false;
    if (apiInitialized) {
      throw std::runtime_error("Multiple AWS SDK initialization not allowed");
    }
    Aws::InitAPI(options);
    apiInitialized = true;
  }
  ~AwsApi() {
    Aws::ShutdownAPI(options);
  }
  Aws::SDKOptions options;
};

struct AwsLogging {
  AwsLogging(const std::shared_ptr <Aws::Utils::Logging::LogSystemInterface> &logSystem) {
    Aws::Utils::Logging::InitializeAWSLogging(logSystem);
  }

  AwsLogging(Aws::Utils::Logging::LogLevel logLevel,
             const std::shared_ptr <Aws::OStream> logFile,
             char const *allocationTag = "") {
    auto logSystem = Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(allocationTag, logLevel, logFile);
    Aws::Utils::Logging::InitializeAWSLogging(logSystem);
  }

  AwsLogging(Aws::Utils::Logging::LogLevel logLevel,
             const Aws::String &filenamePrefix,
             char const *allocationTag = "") {
    auto logSystem = Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(allocationTag, logLevel, filenamePrefix);
    Aws::Utils::Logging::InitializeAWSLogging(logSystem);
  }

  ~AwsLogging() {
    Aws::Utils::Logging::ShutdownAWSLogging();
  }
};
}

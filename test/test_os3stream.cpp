/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "awslabs/enhanced/Aws.h"
#include "awslabs/enhanced/s3stream.h"

#include "gtest/gtest.h"
#include "test_helpers.h"

namespace {
class os3sIntegrationTest : public ::testing::Test {
protected:
  AwsLabs::Enhanced::AwsApi sdk;
  testInfra infra;
};

TEST_F(os3sIntegrationTest, DefaultConstructOs3sCanOpenAnObjectPassingIndividualParams) {
  std::string test_object_name = "default_put";
  infra.register_test_object(test_object_name);
  AwsLabs::Enhanced::os3stream os3s;
  auto s3b = os3s.rdbuf();
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open yet.";
  os3s.set_region(infra.m_region);
  os3s.set_bucket(infra.m_bucket_name);
  os3s.open(test_object_name);
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now.";
  ASSERT_TRUE(os3s.is_open()) << "os3s should be open now.";
  os3s.close();
  ASSERT_FALSE(os3s.is_open()) << "os3s should not be open now.";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now.";

}

TEST_F(os3sIntegrationTest, OpenAnObjectPassingAllParamsToOpen) {
  std::string test_object_name = "open_all_args";
  infra.register_test_object(test_object_name);

  AwsLabs::Enhanced::os3stream os3s;
  auto s3b = os3s.rdbuf();
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open yet";
  os3s.open(infra.m_region, infra.m_bucket_name, test_object_name);
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now";
  ASSERT_TRUE(os3s.is_open()) << "os3s should be open now";
  os3s.close();
  ASSERT_FALSE(os3s.is_open()) << "os3s should not be open now";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now";
}

TEST_F(os3sIntegrationTest, OpenAnObjectPassingAllParamsToConstructor) {
  std::string test_object_name = "const_all_args";
  infra.register_test_object(test_object_name);

  AwsLabs::Enhanced::os3stream os3s(infra.m_region, infra.m_bucket_name, test_object_name);
  auto s3b = os3s.rdbuf();
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now";
  ASSERT_TRUE(os3s.is_open()) << "os3s should be open now";
  os3s.close();
  ASSERT_FALSE(os3s.is_open()) << "os3s should not be open now";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now";
}

TEST_F(os3sIntegrationTest, ClosingAnOs3sThatWAsNotOpenSetsTheFailbitFlag) {
  AwsLabs::Enhanced::os3stream os3s;
  os3s.close();
  ASSERT_TRUE(os3s.fail()) << "Wrong close wasn't marked as fail";
  ASSERT_FALSE(os3s.is_open()) << "os3s should not be open now";
}

TEST_F(os3sIntegrationTest, ExchangeTwoOs3sSucceeds) {
  std::string test_object_name = "swap_2";
  infra.register_test_object(test_object_name);

  AwsLabs::Enhanced::os3stream os3s_1(infra.m_region, infra.m_bucket_name, test_object_name);
  AwsLabs::Enhanced::os3stream os3s_2;
  ASSERT_TRUE(os3s_1.is_open()) << "os3s_1 should be open now";
  ASSERT_FALSE(os3s_2.is_open()) << "os3s_2 should not be open now";
  std::swap(os3s_1, os3s_2);
  ASSERT_FALSE(os3s_1.is_open()) << "os3s_1 should not be open now";
  ASSERT_TRUE(os3s_2.is_open()) << "os3s_2 should be open now";
}

TEST_F(os3sIntegrationTest, WriteAndReadBackSucceeds) {
  std::string test_object_name = "write-read";
  infra.register_test_object(test_object_name);
  //write an object
  {
    AwsLabs::Enhanced::os3stream os3s(infra.m_region, infra.m_bucket_name, test_object_name);
    ASSERT_TRUE(os3s.is_open()) << "os3s should be open now";
    std::string content = "TEST CONTENT";
    os3s << content;
    os3s.close();
  }
  std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  //read the object
  {
    AwsLabs::Enhanced::is3stream is3s(infra.m_region, infra.m_bucket_name, test_object_name);
    ASSERT_TRUE(is3s.is_open()) << "is3s should be open now";
    std::string result;
    is3s >> result;
    ASSERT_FALSE(result.compare("TEST")) << "First word extracted is TEST";
    is3s >> result;
    ASSERT_FALSE(result.empty()) << "A second word is also recovered";
    is3s.close();
  }
}
}
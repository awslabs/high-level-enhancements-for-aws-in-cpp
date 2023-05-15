/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "awslabs/enhanced/Aws.h"
#include "awslabs/enhanced/s3stream.h"

#include "gtest/gtest.h"
#include "test_helpers.h"

namespace {
class is3sIntegrationTest : public ::testing::Test {
protected:
  AwsLabs::Enhanced::AwsApi sdk;
  testInfra infra;
};

TEST_F(is3sIntegrationTest, is3sOpenAndCloseSucceeds) {
  AwsLabs::Enhanced::is3stream is3s;
  auto s3b = is3s.rdbuf();
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open yet";
  is3s.set_region(infra.m_region);
  is3s.set_bucket(infra.m_bucket_name);
  is3s.open(infra.m_object_name);
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now";
  ASSERT_TRUE(is3s.is_open()) << "is3s should be open now";
  is3s.close();
  ASSERT_FALSE(is3s.is_open()) << "is3s should not be open now";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now";
}

TEST_F(is3sIntegrationTest, OpenWithAllArgPassedToOpen) {
  AwsLabs::Enhanced::is3stream is3s;
  auto s3b = is3s.rdbuf();
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open yet";
  is3s.open(infra.m_region, infra.m_bucket_name, infra.m_object_name);
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now";
  ASSERT_TRUE(is3s.is_open()) << "is3s should be open now";
  is3s.close();
  ASSERT_FALSE(is3s.is_open()) << "is3s should not be open now";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now";
}

TEST_F(is3sIntegrationTest, OpenWithAllArgPassedToConstructor) {
  AwsLabs::Enhanced::is3stream is3s(infra.m_region, infra.m_bucket_name, infra.m_object_name);
  auto s3b = is3s.rdbuf();
  ASSERT_TRUE(s3b->is_open()) << "s3b should be open now";
  ASSERT_TRUE(is3s.is_open()) << "is3s should be open now";
  is3s.close();
  ASSERT_FALSE(is3s.is_open()) << "is3s should not be open now";
  ASSERT_FALSE(s3b->is_open()) << "s3b should not be open now";
}
// closing an is3stream that was not open sets fail bit flag
TEST_F(is3sIntegrationTest, ClosingNonOpenedIs3sSetsFailbitFlag) {
  AwsLabs::Enhanced::is3stream is3s;
  is3s.close();
  ASSERT_TRUE(is3s.fail()) << "Wrong close wasn't marked as fail";
  ASSERT_FALSE(is3s.is_open()) << "is3s should not be open now";
}

TEST_F(is3sIntegrationTest, TwoIs3sCanBeExchanged) {
  AwsLabs::Enhanced::is3stream is3s_1(infra.m_region, infra.m_bucket_name, infra.m_object_name);
  AwsLabs::Enhanced::is3stream is3s_2;
  ASSERT_TRUE(is3s_1.is_open()) << "is3s_1 should be open now";
  ASSERT_FALSE(is3s_2.is_open()) << "is3s_2 should not be open now";
  swap(is3s_1, is3s_2);
  ASSERT_FALSE(is3s_1.is_open()) << "is3s_1 should not be open now";
  ASSERT_TRUE(is3s_2.is_open()) << "is3s_2 should be open now";
}

TEST_F(is3sIntegrationTest, AnIs3sCanBeMoved) {
  AwsLabs::Enhanced::is3stream is3s_1(infra.m_region, infra.m_bucket_name, infra.m_object_name);
  ASSERT_TRUE(is3s_1.is_open()) << "is3s_1 should be open now";
  AwsLabs::Enhanced::is3stream is3s_2;
  is3s_2 = std::move(is3s_1);
  ASSERT_TRUE(is3s_2.is_open()) << "is3s_2 should be open now";
}

TEST_F(is3sIntegrationTest, CanReadFirstWordFromIs3s) {
  AwsLabs::Enhanced::is3stream is3s(infra.m_region, infra.m_bucket_name, infra.m_object_name);
  ASSERT_TRUE(is3s.is_open()) << "is3s should be open now";
  std::string result;
  is3s >> result;
  ASSERT_FALSE(result.compare("Test")) << "First word extracted is Test";
  is3s >> result;
  ASSERT_FALSE(result.empty()) << "Second word is also extracted";
}
}
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "awslabs/enhanced/Aws.h"
#include "awslabs/enhanced/s3buf.h"

#include "gtest/gtest.h"
#include "test_helpers.h"

namespace {

class s3bufIntegrationTest : public ::testing::Test {
protected:
  AwsLabs::Enhanced::AwsApi sdk;
  testInfra infra;
};

TEST_F(s3bufIntegrationTest, EnforceTypes) {
  static_assert(std::is_same_v<AwsLabs::Enhanced::s3buf::char_type, std::streambuf::char_type>);
  static_assert(std::is_same_v<AwsLabs::Enhanced::s3buf::traits_type, std::streambuf::traits_type>);
  static_assert(std::is_same_v<AwsLabs::Enhanced::s3buf::pos_type, std::streambuf::pos_type>);
  static_assert(std::is_same_v<AwsLabs::Enhanced::s3buf::int_type, std::streambuf::int_type>);
  static_assert(std::is_same_v<AwsLabs::Enhanced::s3buf::off_type, std::streambuf::off_type>);
}

TEST_F(s3bufIntegrationTest, ConstructAndCloseFails) {
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //close expected to fail
  ASSERT_FALSE(s3b.close()) << "Closing a s3buf that was not open should fail and return nullptr.";
}

TEST_F(s3bufIntegrationTest, OpenCloseFailsForNonInOutModes) {
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open fails with append mode
  s3b.open("any_region", "any_bucket", "any_object", std::ios_base::app);
  ASSERT_FALSE(s3b.is_open()) << "Failed to open in s3b.";
}

TEST_F(s3bufIntegrationTest, ClosingTwiceAfterOpenFailsForInput) {
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "s3buf should be marked open after opening it.";
  //close
  ASSERT_TRUE(s3b.close()) << "Failed to close an open s3buf";
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "s3buf shouldn't be marked open after closing it.";
  //second close expected to fail
  ASSERT_FALSE(s3b.close()) << "Closing a closed s3b should fail and return nullptr.";
}

TEST_F(s3bufIntegrationTest, DoubleOpenFailsForInput) {
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  //open twice expected to fail
  ASSERT_FALSE(s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in))
              << "Double open should fail and return nullptr";
}

TEST_F(s3bufIntegrationTest, OpenCloseOpenSucceedsForInput) {
  //create s3buf with initialization
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  //close
  ASSERT_TRUE(s3b.close()) << "Failed to close an open s3buf";
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Closed s3b shouldn't be marked open.";
  //s3b buff can be opened again
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in))
              << "Failed to open in s3b after closing it.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
}

TEST_F(s3bufIntegrationTest, OpenDoubleCloseFailsForOutput) {
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  std::string test_object_name = "open_double_close";
  infra.register_test_object(test_object_name);
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "s3buf should be marked open after opening it.";
  //close
  ASSERT_TRUE(s3b.close()) << "Failed to close an open s3buf";
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "s3buf shouldn't be marked open after closing it.";
  //second close expected to fail
  ASSERT_FALSE(s3b.close()) << "Closing a closed s3b should fail and return nullptr.";
}

TEST_F(s3bufIntegrationTest, DoubleOpenFailsForOutput) {
  std::string test_object_name = "open_double_open";
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  //open twice expected to fail
  ASSERT_FALSE(s3b.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Double open should fail and return nullptr";
  //even mixed
  //create default s3buf
  AwsLabs::Enhanced::s3buf s3b_IO;
  //s3b buff is not open
  ASSERT_FALSE(s3b_IO.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b_IO.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b_IO.is_open()) << "Failed to mark open s3buf as open.";
  //open twice expected to fail
  ASSERT_FALSE(s3b_IO.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::in))
              << "Double open should fail and return nullptr";
}

TEST_F(s3bufIntegrationTest, OpenCloseOpenSucceedsForOutput) {
  std::string test_object_name = "open_close_open-success";
  //create s3buf with initialization
  AwsLabs::Enhanced::s3buf s3b;
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Default construction of s3b shouldn't be marked open.";
  //open
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Failed to open in s3b.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  //close
  ASSERT_TRUE(s3b.close()) << "Failed to close an open s3buf";
  //s3b buff is not open
  ASSERT_FALSE(s3b.is_open()) << "Closed s3b shouldn't be marked open.";
  //s3b buff can be opened again
  ASSERT_TRUE(s3b.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out))
              << "Failed to open in s3b after closing it.";
  //s3b buff is open
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
}

TEST_F(s3bufIntegrationTest, ExchangeOpenWithCloseS3Bufs) {
  //create open s3buf
  AwsLabs::Enhanced::s3buf s3b_o;
  s3b_o.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in);
  //s3b_o buff is open
  ASSERT_TRUE(s3b_o.is_open()) << "Failed to mark open s3b_o as open.";
  //create close s3buf
  AwsLabs::Enhanced::s3buf s3b_c;
  //s3b_c buff is close
  ASSERT_FALSE(s3b_c.is_open()) << "Failed to mark closed s3b_c as close.";
  // exchange content between the 2 s3bufs
  std::swap(s3b_o, s3b_c);
  ASSERT_TRUE(s3b_c.is_open()) << "Failed to mark open s3b_c as open.";
  ASSERT_FALSE(s3b_o.is_open()) << "Failed to mark closed s3b_o as close.";
}

TEST_F(s3bufIntegrationTest, S3BufCanBeMoveAssigned) {
  //create open s3buf
  AwsLabs::Enhanced::s3buf s3b_o;
  s3b_o.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in);
  //s3b_o buff is open
  ASSERT_TRUE(s3b_o.is_open()) << "Failed to mark open s3b_o as open.";
  //create close s3buf
  AwsLabs::Enhanced::s3buf s3b_c;
  //s3b_c buff is close
  ASSERT_FALSE(s3b_c.is_open()) << "Failed to mark closed s3b_c as close.";
  // move content and set the one moved to uninitialized state
  s3b_c = std::move(s3b_o);
  ASSERT_TRUE(s3b_c.is_open()) << "Failed to mark open s3b_c as open.";
}

TEST_F(s3bufIntegrationTest, S3BufCanBeMoveConstructed) {
  //create open s3buf
  AwsLabs::Enhanced::s3buf s3b_o;
  s3b_o.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in);
  //s3b_o buff is open
  ASSERT_TRUE(s3b_o.is_open()) << "Failed to mark open s3b_o as open.";
  //create a new buffer moving the old one into it, old one has to be uninitialized now
  AwsLabs::Enhanced::s3buf s3b_c(std::move(s3b_o));
  ASSERT_TRUE(s3b_c.is_open()) << "Failed to mark open s3b_c as open.";
}

TEST_F(s3bufIntegrationTest, FourCharsReadFromS3BufSucceeds) {
  AwsLabs::Enhanced::s3buf s3b;
  s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in);
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  //T
  auto c = s3b.sgetc();
  ASSERT_TRUE((std::char_traits<char>::eof() != c)) << "First character is EOF";
  ASSERT_TRUE(('T' == (char) c)) << "First character is not a 'T'";
  //e
  c = s3b.snextc();
  ASSERT_TRUE((std::char_traits<char>::eof() != c)) << "Second character is EOF";
  ASSERT_TRUE(('e' == (char) c)) << "Second character is not a 'e'";
  //getting e again on sbumpc
  c = s3b.sbumpc();
  ASSERT_TRUE((std::char_traits<char>::eof() != c)) << "Second character is EOF";
  ASSERT_TRUE(('e' == (char) c)) << "Second character is not a 'e'";
  //s
  c = s3b.sgetc();
  ASSERT_TRUE((std::char_traits<char>::eof() != c)) << "Third character is EOF";
  ASSERT_TRUE(('s' == (char) c)) << "Third character is not a 's'";
  //t
  c = s3b.snextc();
  ASSERT_TRUE((std::char_traits<char>::eof() != c)) << "Fourth character is EOF";
  ASSERT_TRUE(('t' == (char) c)) << "Fourth character is not a 't'";
}

TEST_F(s3bufIntegrationTest, ReadingEnoughFromS3BufGetsToEOF) {
  AwsLabs::Enhanced::s3buf s3b;
  s3b.open(infra.m_region, infra.m_bucket_name, infra.m_object_name, std::ios_base::in);
  ASSERT_TRUE(s3b.is_open()) << "Failed to mark open s3buf as open.";
  for (int i = 0; i < 1000; i++) {
    auto c = s3b.sbumpc();
    if (c == std::char_traits<char>::eof()) {
      break;
    }
    ASSERT_TRUE(i != 999) << "Read 1000 characters an didn't find a EOF";
  }
}

TEST_F(s3bufIntegrationTest, WritingAndReadingShortStringBackSucceeds) {
  std::string test_object_name = "put_get";
  std::string test_object_content = "Test content";
  infra.register_test_object(test_object_name);

  AwsLabs::Enhanced::s3buf s3b_out;
  s3b_out.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out);
  for (int i = 0; i < test_object_content.size(); i++) {
    s3b_out.sputc(test_object_content[i]);
  }
  s3b_out.close();
  std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  AwsLabs::Enhanced::s3buf s3b_in;
  s3b_in.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::in);
  ASSERT_TRUE('T' == (char) s3b_in.sbumpc()) << "Failed to get the first character uploaded";
  ASSERT_TRUE('e' == (char) s3b_in.sbumpc()) << "Failed to get the second character uploaded";
  ASSERT_TRUE('s' == (char) s3b_in.sbumpc()) << "Failed to get the third character uploaded";
  ASSERT_TRUE('t' == (char) s3b_in.sbumpc()) << "Failed to get the fourth character uploaded";
}

TEST_F(s3bufIntegrationTest, WritingAndReadingLongStringBackSucceeds) {
  std::string test_object_name = "put_get";
  std::string test_object_content = "Test content has more than 64 characters because I wrote "
                                    "1 2 3 4 5 6 7 8 9 0 "
                                    "1 2 3 4 5 6 7 8 9 0 "
                                    "1 2 3 4 5 6 7 8 9 0 "
                                    "1 2 3 4 5 6 7 8 9 0 "
                                    "1 2 3 4 5 6 7 8 9 0 ";
  infra.register_test_object(test_object_name);
  AwsLabs::Enhanced::s3buf s3b_out;
  s3b_out.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::out);
  for (int i = 0; i < test_object_content.size(); i++) {
    s3b_out.sputc(test_object_content[i]);
  }
  s3b_out.close();
  std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  AwsLabs::Enhanced::s3buf s3b_in;
  s3b_in.open(infra.m_region, infra.m_bucket_name, test_object_name, std::ios_base::in);
  std::stringstream ss;
  do {
    char ch = s3b_in.sgetc();
    ss.put(ch);
  } while (s3b_in.snextc() != EOF);

  // FIX: an extra character is introduced by calling to S3, a \0
  ASSERT_TRUE(test_object_content.size() + 1 == ss.str().size())
              << "Failed to get the same amount of characters as put";

}
}
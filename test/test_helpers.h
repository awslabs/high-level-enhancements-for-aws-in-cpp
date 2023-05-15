/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef S3STREAM_INCLUDE_TEST_HELPERS_H
#define S3STREAM_INCLUDE_TEST_HELPERS_H

#include <aws/core/Aws.h>
#include <aws/core/utils/UUID.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>

class testInfra {
  Aws::S3::S3Client _client;
  std::vector<std::string> m_objects_to_delete;

  void deleteObject(const std::string &object_name) {
    Aws::S3::Model::DeleteObjectRequest objectRequest;
    objectRequest.SetBucket(m_bucket_name);
    objectRequest.SetKey(object_name);
    _client.DeleteObject(objectRequest);
  }

public:
  Aws::String m_bucket_name;
  const Aws::String m_object_name;
  const Aws::String m_region;

  testInfra() : m_object_name("testObject"), m_region("us-east-1") {
    // Create S3 Client and bucket name
    Aws::String uuid = Aws::Utils::UUID::RandomUUID();
    m_bucket_name = "testing-bucket-" + Aws::Utils::StringUtils::ToLower(uuid.c_str());
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "us-east-1";
    _client = Aws::S3::S3Client(clientConfig);
    // Create bucket
    {
      Aws::S3::Model::CreateBucketRequest bucketRequest;
      bucketRequest.SetBucket(m_bucket_name);
      Aws::S3::Model::CreateBucketOutcome outcome = _client.CreateBucket(bucketRequest);
      if (!outcome.IsSuccess()) {
        auto err = outcome.GetError();
        std::cout << "Error on CreateBucket: " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
        throw std::runtime_error(outcome.GetError().GetMessage().c_str());
      } else {
        std::cout << "Created bucket " << m_bucket_name << std::endl;
      }
    }
    // Create object
    {
      Aws::S3::Model::PutObjectRequest objectRequest;
      objectRequest.SetBucket(m_bucket_name);
      objectRequest.SetKey(m_object_name);
      std::shared_ptr<Aws::IOStream> input_data =
          Aws::MakeShared<Aws::StringStream>("SampleAllocationTag", "Test Content");
      objectRequest.SetBody(input_data);

      Aws::S3::Model::PutObjectOutcome outcome = _client.PutObject(objectRequest);
      if (outcome.IsSuccess()) {
        std::cout << "Added object '" << m_object_name << "' to bucket '" << m_bucket_name << "'." << std::endl;
      } else {
        std::cout << "Error on PutObject: " << outcome.GetError().GetMessage() << std::endl;
        throw std::runtime_error(outcome.GetError().GetMessage());
      }
    }
  }

  void register_test_object(const std::string &object_name) {
    m_objects_to_delete.push_back(object_name);
  }

  ~testInfra() {
    // Delete default test object
    deleteObject(m_object_name);
    // Delete registered test objects
    for (const auto& s : m_objects_to_delete) {
      deleteObject(s);
    }

    // Delete bucket
    {
      Aws::S3::Model::DeleteBucketRequest bucketRequest;
      bucketRequest.SetBucket(m_bucket_name);
      _client.DeleteBucket(bucketRequest);
    }
  }

};

#endif //S3STREAM_INCLUDE_TEST_HELPERS_H

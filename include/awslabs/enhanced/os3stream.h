// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef S3STREAM_INCLUDE_OS3STREAM_H
#define S3STREAM_INCLUDE_OS3STREAM_H

#include <awslabs/enhanced/s3buf.h>
#include <string>
#include <ostream>

namespace AwsLabs::Enhanced {
class os3stream : public std::ostream {
  AwsLabs::Enhanced::s3buf *_s3b = nullptr;
  std::string _region;
  std::string _bucket_name;
  std::string _object_name;
public:
  /**
   * Constructs an os3stream object that is not associated with any object or bucket.
   */
  os3stream() : std::ostream(new AwsLabs::Enhanced::s3buf()) {
    _s3b = dynamic_cast<AwsLabs::Enhanced::s3buf *>(std::ostream::rdbuf());
  }
  /**
   * Copy constructor is deleted.
   */
  os3stream(const os3stream &) = delete;
  /**
   * Move constructor deleted
   * @param os3s
   */
  os3stream(os3stream &&os3s) = delete;
  /**
   * Copy assignment is deleted.
   * @return
   */
  os3stream &operator=(const os3stream &) = delete;
  /**
   * Move assignment acquires os3s contents.
   * @param os3s
   * @return
   */
  os3stream &operator=(os3stream &&os3s) {
    _s3b->close();
    _region = os3s._region;
    _bucket_name = os3s._bucket_name;
    _object_name = os3s._object_name;
    os3s._region = "";
    os3s._bucket_name = "";
    os3s._object_name = "";
    std::swap(*_s3b, *(os3s._s3b));
    return *this;
  }
  /**
   * Construct an os3stream that objects will be open in bucket_name bucket.
   * @param bucket_name
   */
  os3stream(const std::string &bucket_name) {}
  /**
   * Construct an os3stream and opens the target object for writing.
   * @param bucket_name
   * @param object_name
   */
  os3stream(const std::string region, const std::string &bucket_name, const std::string &object_name)
      : std::ostream(new AwsLabs::Enhanced::s3buf()) {
    _s3b = dynamic_cast<AwsLabs::Enhanced::s3buf *>(std::ostream::rdbuf());
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    _s3b->open(_region, _bucket_name, _object_name, std::ios_base::out);
  }
  /**
   * Sets the region where objects will be write to.
   * @param region
   */
  void set_region(const std::string &region) {
    _region = region;
  }
  /**
   * Sets the bucket where objects will be write to.
   * @param bucket_name
   */
  void set_bucket(const std::string &bucket_name) {
    _bucket_name = bucket_name;
  }
  /**
   * Opens object_name in previously set region and bucket for writing content into it.
   * @param object_name
   */
  void open(const std::string &object_name) {
    _object_name = object_name;
    _s3b->open(_region, _bucket_name, _object_name, std::ios_base::out);
  }
  /**
   * Opens object_name in bucket_name for writing content into it.
   * @param region
   * @param bucket_name
   * @param object_name
   */
  void open(const std::string &region, const std::string &bucket_name, const std::string &object_name) {
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    _s3b->open(_region, _bucket_name, _object_name, std::ios_base::out);
  }
  /**
   * Returns whether the os3stream is currently associated to an S3 object.
   * @return
   */
  bool is_open() const {
    return _s3b->is_open();
  }
  /**
   * Closes the object currently associated with the object by completing the object put operations.
   * and it disassociates the object from the stream.
   * If the os3stream is currently not associated with any object, calling this function fails.
   * Note: destruction of the os3stream automatically closes it.
   */
  void close() {
    if (!_s3b->close()) {
      std::ios::setstate(failbit);
    }
  }
  /**
   * Returns a pointer to the internal s3buf object.
   * @return
   */
  s3buf *rdbuf() const {
    return _s3b;
  }
  /**
   * Exchange all internals between *this and os3s
   * @param os3s
   */
  void swap(os3stream &os3s) {
    std::swap(_s3b, os3s._s3b);
    std::swap(_region, os3s._region);
    std::swap(_bucket_name, os3s._bucket_name);
    std::swap(_object_name, os3s._object_name);
  }

  virtual ~os3stream() {
    delete _s3b;
  }
};
}

namespace std {
/**
 * swaps internal content between lhs and rhs os3streams.
 * @param lhs
 * @param rhs
 */
void swap(AwsLabs::Enhanced::os3stream &lhs, AwsLabs::Enhanced::os3stream &rhs) {
  lhs.swap(rhs);
}
}

#endif //S3STREAM_INCLUDE_OS3STREAM_H

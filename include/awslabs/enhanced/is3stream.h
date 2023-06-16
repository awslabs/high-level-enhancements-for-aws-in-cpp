// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef S3STREAM_INCLUDE_IS3STREAM_H
#define S3STREAM_INCLUDE_IS3STREAM_H

#include <awslabs/enhanced/s3buf.h>
#include <string>
#include <string_view>
#include <istream>

namespace AwsLabs::Enhanced {
class is3stream : public std::istream {
  s3buf *_s3b = nullptr;
  std::string _region;
  std::string _bucket_name;
  std::string _object_name;
protected:
  /**
   * Opens the current region/bucket/object and set the iostate
  */
  void open(std::ios_base::openmode mode = std::ios_base::in) {
    if(_s3b->open(_region, _bucket_name, _object_name, mode))
      std::ios::setstate(goodbit);
    else
      std::ios::setstate(failbit);
  }
public:
  /**
   * Default constructor for a is3stream not associated to an S3 object
   *
   */
  is3stream() : std::istream(new AwsLabs::Enhanced::s3buf()) {
    _s3b = dynamic_cast<AwsLabs::Enhanced::s3buf *>(std::istream::rdbuf());
  }
  /**
   * Copy constructor is deleted.
   */
  is3stream(const is3stream &) = delete;
  /**
   * Move constructor deleted
   * @param is3s
   */
  is3stream(is3stream &&is3s) = delete;
  /**
 * Copy assignment is deleted.
 * @return
 */
  is3stream &operator=(const is3stream &) = delete;
  /**
 * Move assignment acquires is3s contents.
 * @param is3s
 * @return
 */
  is3stream &operator=(is3stream &&is3s) {
    if(is_open())
      _s3b->close();
    _region = is3s._region;
    _bucket_name = is3s._bucket_name;
    _object_name = is3s._object_name;
    is3s._region = "";
    is3s._bucket_name = "";
    is3s._object_name = "";
    std::swap(*_s3b, *(is3s._s3b));
    return *this;
  }
  /**
   * Construct an is3stream and opens the target object for reading.
   * @param region
   * @param bucket_name
   * @param object_name
   */
  is3stream(const std::string &region, const std::string &bucket_name, const std::string &object_name)
      : std::istream(new s3buf()) {
    _s3b = dynamic_cast<s3buf *>(std::istream::rdbuf());
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    open();
  }

  /**
   * Sets the region where objects will be read from.
   * @param region
   */
  void set_region(const std::string &region) {
    _region = region;
  }

  /**
   * Sets the bucket where objects will be read from.
   * @param bucket_name
   */
  void set_bucket(const std::string &bucket_name) {
    _bucket_name = bucket_name;
  }
  /**
   * Opens object_name in previously set region and bucket for reading content from it.
   * @param object_name
   */
  void open(const std::string &object_name) {
    _object_name = object_name;
    open();
  }
  /**
   * Opens object_name in previously set region and bucket to read content from it.
   * @param region
   * @param bucket_name
   * @param object_name
   */
  void open(const std::string &region, const std::string &bucket_name, const std::string &object_name) {
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    open();
  }
  /**
   * Returns whether the is3stream is currently associated to an S3 object.
   * @return
   */
  bool is_open() const {
    return _s3b->is_open();
  }
  /**
 * Closes the object currently associated with the object. No write backs are provided for is3streams.
 * If the is3stream is currently not associated with any object, calling this function fails.
 * Note: destruction of the is3stream automatically closes it.
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
 * Exchange all internals between *this and is3s
 * @param is3s
 */
  void swap(is3stream &is3s) {
    std::swap(_s3b, is3s._s3b);
    std::swap(_region, is3s._region);
    std::swap(_bucket_name, is3s._bucket_name);
    std::swap(_object_name, is3s._object_name);
  }

  virtual ~is3stream() {
    try {
      delete _s3b;
    } catch(...) {
      // Don't throw from destructors
      // https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Re-never-fail
    }
  }
};

/**
 * swaps internal content between lhs and rhs is3streams.
 * @param lhs
 * @param rhs
 */
void swap(is3stream &lhs, is3stream &rhs) {
  lhs.swap(rhs);
}
}

#endif //S3STREAM_INCLUDE_IS3STREAM_H

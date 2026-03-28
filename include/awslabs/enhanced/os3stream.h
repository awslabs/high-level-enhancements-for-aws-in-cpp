// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef S3STREAM_INCLUDE_OS3STREAM_H
#define S3STREAM_INCLUDE_OS3STREAM_H

#include <awslabs/enhanced/s3buf.h>
#include <ostream>
#include <string>

namespace AwsLabs::Enhanced {

namespace detail {
/// Private base class that owns the s3buf as an in-place value member.
///
/// C++ initialises base classes in declaration order, so os3buf_holder
/// (listed first in os3stream's base list) is fully constructed before
/// std::ostream's constructor runs and receives a pointer to _buf.
/// This eliminates the manual new/delete that was previously required to
/// work around the initialisation-order constraint.
struct os3buf_holder {
  s3buf _buf;
};
} // namespace detail

class os3stream : private detail::os3buf_holder, public std::ostream {
  std::string _region;
  std::string _bucket_name;
  std::string _object_name;

public:
  /**
   * Constructs an os3stream not associated with any S3 object or bucket.
   */
  os3stream() : detail::os3buf_holder{}, std::ostream(&_buf) {}

  /**
   * Copy constructor is deleted.
   */
  os3stream(const os3stream &) = delete;

  /**
   * Move constructor acquires os3s contents.
   *
   * Allocates a fresh s3buf for *this via the holder base, then swaps its
   * contents with os3s's buffer.  The base-class rdbuf pointer in each stream
   * stays fixed; only the data inside the buffers is exchanged.
   * @param os3s  the stream to move from; left in a valid, closed state.
   */
  os3stream(os3stream &&os3s) : detail::os3buf_holder{}, std::ostream(&_buf) {
    _region = std::move(os3s._region);
    _bucket_name = std::move(os3s._bucket_name);
    _object_name = std::move(os3s._object_name);
    std::swap(_buf, os3s._buf);
  }

  /**
   * Copy assignment is deleted.
   */
  os3stream &operator=(const os3stream &) = delete;

  /**
   * Move assignment acquires os3s contents.
   */
  os3stream &operator=(os3stream &&os3s) {
    _buf.close();
    _region = std::move(os3s._region);
    _bucket_name = std::move(os3s._bucket_name);
    _object_name = std::move(os3s._object_name);
    os3s._region = "";
    os3s._bucket_name = "";
    os3s._object_name = "";
    std::swap(_buf, os3s._buf);
    return *this;
  }

  /**
   * Construct an os3stream and open the target object for writing.
   * @param region
   * @param bucket_name
   * @param object_name
   */
  os3stream(const std::string &region, const std::string &bucket_name,
            const std::string &object_name)
      : detail::os3buf_holder{}, std::ostream(&_buf), _region(region), _bucket_name(bucket_name),
        _object_name(object_name) {
    _buf.open(_region, _bucket_name, _object_name, std::ios_base::out);
  }

  /**
   * Sets the region where objects will be written to.
   */
  void set_region(const std::string &region) { _region = region; }

  /**
   * Sets the bucket where objects will be written to.
   */
  void set_bucket(const std::string &bucket_name) { _bucket_name = bucket_name; }

  /**
   * Opens object_name in the previously set region and bucket for writing.
   */
  void open(const std::string &object_name) {
    _object_name = object_name;
    _buf.open(_region, _bucket_name, _object_name, std::ios_base::out);
  }

  /**
   * Opens object_name in bucket_name for writing.
   */
  void open(const std::string &region, const std::string &bucket_name,
            const std::string &object_name) {
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    _buf.open(_region, _bucket_name, _object_name, std::ios_base::out);
  }

  /**
   * Returns whether the os3stream is currently associated with an S3 object.
   */
  bool is_open() const { return _buf.is_open(); }

  /**
   * Closes the current object association, completing any pending PutObject.
   * Fails (sets failbit) if no object is currently open.
   * Note: the destructor automatically closes the stream.
   */
  void close() {
    if (!_buf.close()) {
      std::ios::setstate(failbit);
    }
  }

  /**
   * Returns a pointer to the internal s3buf object.
   */
  s3buf *rdbuf() const { return const_cast<s3buf *>(&_buf); }

  /**
   * Exchange all internals between *this and os3s.
   *
   * Swaps the *contents* of each stream's s3buf (not the pointers), so the
   * base-class rdbuf pointer in each stream continues to point at its own
   * buffer object — writing to either stream after the swap works correctly.
   */
  void swap(os3stream &os3s) {
    std::swap(_buf, os3s._buf);
    std::swap(_region, os3s._region);
    std::swap(_bucket_name, os3s._bucket_name);
    std::swap(_object_name, os3s._object_name);
  }

  /// os3buf_holder's destructor automatically cleans up _buf — no manual
  /// delete is needed or present.
  ~os3stream() = default;
};

} // namespace AwsLabs::Enhanced

namespace std {
/**
 * Swaps internal content between lhs and rhs os3streams.
 */
void swap(AwsLabs::Enhanced::os3stream &lhs, AwsLabs::Enhanced::os3stream &rhs) { lhs.swap(rhs); }
} // namespace std

#endif // S3STREAM_INCLUDE_OS3STREAM_H

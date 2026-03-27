// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef S3STREAM_INCLUDE_IS3STREAM_H
#define S3STREAM_INCLUDE_IS3STREAM_H

#include <awslabs/enhanced/s3buf.h>
#include <istream>
#include <string>
#include <string_view>

namespace AwsLabs::Enhanced {

namespace detail {
/// Private base class that owns the s3buf as an in-place value member.
///
/// C++ initialises base classes in declaration order, so s3buf_holder
/// (listed first in is3stream's base list) is fully constructed before
/// std::istream's constructor runs and receives a pointer to _buf.
/// This eliminates the manual new/delete that was previously required to
/// work around the initialisation-order constraint.
struct is3buf_holder {
  s3buf _buf;
};
} // namespace detail

class is3stream : private detail::is3buf_holder, public std::istream {
  std::string _region;
  std::string _bucket_name;
  std::string _object_name;

protected:
  /**
   * Opens the current region/bucket/object and sets the iostate.
   */
  void open(std::ios_base::openmode mode = std::ios_base::in) {
    if (_buf.open(_region, _bucket_name, _object_name, mode))
      std::ios::setstate(goodbit);
    else
      std::ios::setstate(failbit);
  }

public:
  /**
   * Default constructor for an is3stream not associated with any S3 object.
   */
  is3stream() : detail::is3buf_holder{}, std::istream(&_buf) {}

  /**
   * Copy constructor is deleted.
   */
  is3stream(const is3stream &) = delete;

  /**
   * Move constructor acquires is3s contents.
   *
   * Allocates a fresh s3buf for *this via the holder base, then swaps its
   * contents with is3s's buffer.  The base-class rdbuf pointer in each stream
   * stays fixed (pointing at its own in-place buffer); only the data inside
   * the buffers is exchanged.
   * @param is3s  the stream to move from; left in a valid, closed state.
   */
  is3stream(is3stream &&is3s) : detail::is3buf_holder{}, std::istream(&_buf) {
    _region = std::move(is3s._region);
    _bucket_name = std::move(is3s._bucket_name);
    _object_name = std::move(is3s._object_name);
    std::swap(_buf, is3s._buf);
  }

  /**
   * Copy assignment is deleted.
   */
  is3stream &operator=(const is3stream &) = delete;

  /**
   * Move assignment acquires is3s contents.
   * @param is3s
   * @return
   */
  is3stream &operator=(is3stream &&is3s) {
    if (is_open())
      _buf.close();
    _region = std::move(is3s._region);
    _bucket_name = std::move(is3s._bucket_name);
    _object_name = std::move(is3s._object_name);
    is3s._region = "";
    is3s._bucket_name = "";
    is3s._object_name = "";
    std::swap(_buf, is3s._buf);
    return *this;
  }

  /**
   * Construct an is3stream and open the target object for reading.
   * @param region
   * @param bucket_name
   * @param object_name
   */
  is3stream(const std::string &region, const std::string &bucket_name,
            const std::string &object_name)
      : detail::is3buf_holder{}, std::istream(&_buf), _region(region), _bucket_name(bucket_name),
        _object_name(object_name) {
    open();
  }

  /**
   * Sets the region where objects will be read from.
   */
  void set_region(const std::string &region) { _region = region; }

  /**
   * Sets the bucket where objects will be read from.
   */
  void set_bucket(const std::string &bucket_name) { _bucket_name = bucket_name; }

  /**
   * Opens object_name in the previously set region and bucket for reading.
   */
  void open(const std::string &object_name) {
    _object_name = object_name;
    open();
  }

  /**
   * Opens object_name in bucket_name for reading.
   */
  void open(const std::string &region, const std::string &bucket_name,
            const std::string &object_name) {
    _region = region;
    _bucket_name = bucket_name;
    _object_name = object_name;
    open();
  }

  /**
   * Returns whether the is3stream is currently associated with an S3 object.
   */
  bool is_open() const { return _buf.is_open(); }

  /**
   * Closes the current object association.
   * No write-back is performed for is3streams.
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
   * Exchange all internals between *this and is3s.
   *
   * Swaps the *contents* of each stream's s3buf (not the pointers), so the
   * base-class rdbuf pointer in each stream continues to point at its own
   * buffer object — reading from either stream after the swap works correctly.
   */
  void swap(is3stream &is3s) {
    std::swap(_buf, is3s._buf);
    std::swap(_region, is3s._region);
    std::swap(_bucket_name, is3s._bucket_name);
    std::swap(_object_name, is3s._object_name);
  }

  /// s3buf_holder's destructor automatically cleans up _buf — no manual
  /// delete is needed or present.
  ~is3stream() = default;
};

/**
 * Swaps internal content between lhs and rhs is3streams.
 */
void swap(is3stream &lhs, is3stream &rhs) { lhs.swap(rhs); }
} // namespace AwsLabs::Enhanced

#endif // S3STREAM_INCLUDE_IS3STREAM_H

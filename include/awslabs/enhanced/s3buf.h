// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef S3STREAM_INCLUDE_S3BUF_H
#define S3STREAM_INCLUDE_S3BUF_H

#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <streambuf>
#include <variant>
#include <memory>

namespace AwsLabs::Enhanced {

struct s3location {
  s3location(const char *region, const char *bucket, const char *object)
      : region(region), bucket(bucket), object(object) {}
  std::string region;
  std::string bucket;
  std::string object;
};

class s3buf : public std::streambuf {
  using get_outcome_t = Aws::S3::Model::GetObjectOutcome;
  using internal_gbuf_t = std::variant<get_outcome_t>;

  std::unique_ptr <internal_gbuf_t> internal_gbuf = nullptr;
  std::shared_ptr <std::stringstream> internal_pbuf = nullptr;
  char *get_buffer = nullptr;
  char *put_buffer = nullptr;
  const size_t buffer_size = 64;
  std::unique_ptr <s3location> _object_loc = nullptr; //location  of the s3 object
  std::unique_ptr <Aws::S3::S3Client> s3_client = nullptr;
public:
  using char_type = std::streambuf::char_type;
  using traits_type = std::streambuf::traits_type;
  using int_type = typename traits_type::int_type;
  using pos_type = typename traits_type::pos_type;
  using off_type = typename traits_type::off_type;

  /**
   * Constructs a s3buf without associating to any s3 object.
   */
  s3buf() {
    //set the pointers for get
    setg(nullptr, nullptr, nullptr);
  }
  /**
   * The copy constructor is deleted.
   */
  s3buf(const s3buf &) = delete;
  /**
   * s3b content is acquired.
   * @param s3b
   */
  s3buf(s3buf &&s3b) {
    std::swap(internal_gbuf, s3b.internal_gbuf); //current internal_gbuf is nullptr thus no need to clean it
    std::swap(get_buffer, s3b.get_buffer); //current get_buffer is nullptr thus no need to clean it
    //get the pointers, they need no corrections because get_buffer is outside the object
    setg(s3b.eback(), s3b.gptr(), s3b.egptr());
    s3b.setg(nullptr, nullptr, nullptr);
  }
  /**
   * Move assignment closes s3b before acquiring its contents.
   * @param s3b
   * @return
   */
  s3buf &operator=(s3buf &&s3b) {
    close();
    swap(s3b);
    return *this;
  }
  /**
   * Before destroying, close is automatically called.
   */
  virtual ~s3buf() {
    close();
    if (get_buffer) {
      delete[] get_buffer;
    }
    if (put_buffer) {
      delete[] put_buffer;
    }
  }
  /**
   * Associates the S3 bucket and object to be read/written by the s3buf.
   * Only ios_base::in and ios_base::out are valid values, everything else will fail the operation.
   * Opening an already opened s3buf fails.
   * Return is *this in success, and nullptr in failure
   *
   * @param bucket_name
   * @param object_name
   * @return
   */
  s3buf *open(const char *region, const char *bucket_name, const char *object_name, std::ios_base::openmode mode) {
    _object_loc = std::make_unique<s3location>(region, bucket_name, object_name);
    if (is_open()) {
      return nullptr; // Already opened
    }
    if (!s3_client) {
      Aws::Client::ClientConfiguration config;
      config.region = _object_loc->region;
      s3_client = std::make_unique<Aws::S3::S3Client>(config);
    }
    if (std::ios_base::out == mode) {
      put_buffer = new char[buffer_size]();
      setp(put_buffer, put_buffer + buffer_size);
      return this;
    } else if (std::ios_base::in == mode) {
      //TODO download file in parts
      Aws::S3::Model::GetObjectRequest get_request;
      get_request.SetBucket(_object_loc->bucket);
      get_request.SetKey(_object_loc->object);

      internal_gbuf = std::make_unique<internal_gbuf_t>(s3_client->GetObject(get_request));
      if (get_if<get_outcome_t>(&*internal_gbuf)->IsSuccess()) {
        get_buffer = new char[buffer_size]();
        return this;
      } else {
        internal_gbuf = nullptr;
        return nullptr;
      }
    } else {
      return nullptr;
    }
  }
  /**
   * Convenience call for open using strings
   * @param region
   * @param bucket_name
   * @param object_name
   * @param mode
   * @return
   */
  s3buf *open(const std::string &region,
              const std::string &bucket_name,
              const std::string &object_name,
              std::ios_base::openmode mode) {
    return open(region.c_str(), bucket_name.c_str(), object_name.c_str(), mode);
  }

  /**
   * Returns whether the s3buf is associated to an object or not.
   * @return
   */
  bool is_open() const {
    return get_buffer || put_buffer;
  }
  /**
   * Closes an object association. Any pending output is written and transactions are completed.
   * The function fails if any suboperation fails or there is no open association to an S3 object.
   * @return
   */
  s3buf *close() {
    if (get_buffer) {
      internal_gbuf = nullptr;
      delete[] get_buffer;
      get_buffer = nullptr;
      setg(nullptr, nullptr, nullptr);
      return this;
    } else if (put_buffer) {
      //if empty object with only key, overflow was never called
      if (!internal_pbuf) {
        internal_pbuf = std::make_shared<std::stringstream>();
      }
      for (auto it = pbase(); it < pptr(); ++it) {
        internal_pbuf->put(*it);
      }
      delete[] put_buffer;
      put_buffer = nullptr;
      setp(nullptr, nullptr);
      //TODO upload file in parts
      Aws::S3::Model::PutObjectRequest put_request;
      put_request.SetBucket(_object_loc->bucket);
      put_request.SetKey(_object_loc->object);
      put_request.SetBody(internal_pbuf);
      auto outcome = s3_client->PutObject(put_request);
      internal_pbuf = nullptr;
      if (outcome.IsSuccess()) {
        return this;
      } else {
        return nullptr;
      }
    } else {
      return nullptr;
    }
  }
  /**
 * Exchange the contents between s3b and *this.
 * @param s3b
 */
  void swap(s3buf &s3b) {
    std::swap(internal_gbuf, s3b.internal_gbuf);
    std::swap(get_buffer, s3b.get_buffer);
    std::swap(_object_loc, s3b._object_loc);
    std::swap(put_buffer, s3b.put_buffer);
    char *b = s3b.eback();
    char *n = s3b.gptr();
    char *e = s3b.egptr();
    s3b.setg(eback(), gptr(), egptr());
    setg(b, n, e);
  }
protected:
  /**
   * if nothing left to read returns eof
   * if stuff available in internal_gbuf, it is moved to the get_buffer and the first character returned
   * @return
   */
  virtual int_type underflow() override {
    if (internal_gbuf) {
      auto outcome = std::get_if<get_outcome_t>(&*internal_gbuf);
      if (outcome) {
        int pos = 0;
        while (pos < buffer_size) {
          if (!outcome->GetResult().GetBody().eof()) {
            outcome->GetResult().GetBody().read(get_buffer + pos, 1);
          } else {
            break;
          }
          ++pos;
        }
        setg(get_buffer, get_buffer, get_buffer + pos);
      }
    }
    if (egptr() > gptr() && !(eback() > gptr())) {
      return *gptr();
    } else {
      return traits_type::eof();
    }
  }
  virtual int_type overflow(int_type c) override {
    if (!internal_pbuf) {
      internal_pbuf = std::make_shared<std::stringstream>();
    }
    for (auto it = pbase(); it < pptr(); ++it) {
      internal_pbuf->put(*it);
    }
    setp(put_buffer, put_buffer + buffer_size);
    sputc(c);
    return c;
  }
};
}

namespace std {
void swap(AwsLabs::Enhanced::s3buf &lhs, AwsLabs::Enhanced::s3buf &rhs) {
  lhs.swap(rhs);
}
}
#endif //S3STREAM_INCLUDE_S3BUF_H

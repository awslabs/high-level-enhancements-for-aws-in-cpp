/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LAMBDA_CLIENT_H
#  define LAMBDA_CLIENT_H
#include<memory>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/Array.h>
#include <aws/core/utils/base64/Base64.h>
#include <aws/core/utils/HashingUtils.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <future>
#include <tuple>
#include "detail/lambda_detail.h"

#include <alpaca/alpaca.h>
#ifdef __cpp_lib_expected
#include <expected>
namespace expns = std;
#else
#include <tl/expected.hpp>
namespace expns = tl;
#endif
using Aws::Utils::Json::JsonValue;
using namespace std::string_literals;
#define BIND_AWS_LAMBDA(client, f, name) client.bind_lambda<decltype(f)>(name)

namespace AwsLabs::Enhanced {
struct EnhancedLambdaClient;

template<typename Sig>
struct Lambda;

struct EnhancedLambdaClient {
  EnhancedLambdaClient(Aws::Client::ClientConfiguration config = {}) {
    client = std::make_unique<Aws::Lambda::LambdaClient>(config);
  }
  template<typename Sig>
  Lambda<Sig> bind_lambda(std::string name) {
    return Lambda<Sig>(*this, name);
  }
  std::unique_ptr <Aws::Lambda::LambdaClient> client;
};

void with(JsonValue &payload, std::string key, std::string value) {
  payload.WithString(key, value);
};

void with(JsonValue &payload, std::string key, bool value) {
  payload.WithBool(key, value);
};

void with(JsonValue &payload, std::string key, double value) {
  payload.WithDouble(key, value);
};

void with(JsonValue &payload, std::string key, int64_t value) {
  payload.WithInt64(key, value);
};

void with(JsonValue &payload, std::string key, int value) {
  payload.WithInteger(key, value);
};

template<typename ...Args> struct MarshallArgs {
  template<std::size_t ...I>
  auto impl(std::index_sequence<I...> i, Args... args) {
    JsonValue payload;
    ((with(payload, "key"s + std::to_string(I), args)), ...);
    return payload;
  }
  auto operator()(Args ...args) {
    return impl(std::make_index_sequence<sizeof...(Args)>(), args...);
  }
};

template<>
struct MarshallArgs<Aws::Utils::Json::JsonValue> {
  auto operator()(Aws::Utils::Json::JsonValue val) {
    return val;
  }
};

template<typename Sig>
struct Lambda;

template<typename T> struct Getter;

template<>
struct Getter<JsonValue> {
  JsonValue operator()(JsonValue r) {
    return r;
  }
};

template<>
struct Getter<bool> {
  bool operator()(JsonValue r) {
    return r.View().GetBool("value"s);
  }
};

template<>
struct Getter<double> {
  double operator()(JsonValue r) {
    return r.View().GetDouble("value"s);
  }
};

template<>
struct Getter<int64_t> {
  int64_t operator()(JsonValue r) {
    return r.View().GetInt64("value"s);
  }
};

template<>
struct Getter<int> {
  int operator()(JsonValue r) {
    return r.View().GetInteger("value"s);
  }
};

template<>
struct Getter<std::string> {
  std::string operator()(JsonValue r) {
    return r.View().GetString("value"s);
  }
};

// Precondition: Not an error
template<typename R>
struct Getter<expns::expected < R, std::string>> {
expns::expected <R, std::string> operator()(JsonValue r) {
  return Getter<R>{}(r);
}
};

template<typename R>
struct HandleFunctionError {
  R operator()(JsonValue v) {
    throw std::runtime_error(v.View().GetString("errorMessage"));
  }
};

template<typename R>
struct HandleFunctionError<expns::expected < R, std::string>> {
expns::expected <R, std::string> operator()(JsonValue v) {
  return expns::unexpected(v.View().GetString("errorMessage"));
}
};

template<typename R, typename ...Args>
struct Lambda<R(Args...)> {
  Lambda(EnhancedLambdaClient &client, std::string name)
      : client(client), name(name) {}
  R handleSuccessfulInvocation(Aws::Lambda::Model::InvokeResult &result) {
    Aws::IOStream &payload = result.GetPayload();
    // h/t https://stackoverflow.com/questions/3203452/how-to-read-entire-stream-into-a-stdstring
    Aws::String ret(std::istreambuf_iterator<char>(payload), {});

    if (result.GetFunctionError().length())
      return HandleFunctionError<R>{}(ret);
    JsonValue retAsJson(ret);
    Aws::Utils::ByteBuffer bb = Aws::Utils::HashingUtils::Base64Decode(retAsJson.View().GetString("value"));
    static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Platform breaks our assumption that characters are 8 bits");
    std::vector<uint8_t> bytes(reinterpret_cast<uint8_t *>(bb.GetUnderlyingData()),
                               reinterpret_cast<uint8_t *>(bb.GetUnderlyingData() + bb.GetLength()));
    std::error_code ec;
    auto resultHolder = alpaca::deserialize<Detail::ResultHolder<R>>(bytes, ec);
    if(ec) 
      throw std::runtime_error("Deserialization error code: " + ec.message());
    return resultHolder.result;
  }

  R operator()(Args... args) {
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetFunctionName(name);
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
    Detail::ArgsHolder argsHolder(std::tuple(args...));
    std::vector<uint8_t> bytes;
    auto bytes_written = alpaca::serialize(argsHolder, bytes);
    static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Platform breaks our assumption that characters are 8 bits");
    Aws::Utils::ByteBuffer byteBuffer(reinterpret_cast<unsigned char *>(&bytes.front()), bytes_written);
    Aws::Utils::Json::JsonValue jsonPayload;
    jsonPayload.WithString("serialized",  Aws::Utils::HashingUtils::Base64Encode(byteBuffer));

    std::shared_ptr <Aws::IOStream> payload
        = Aws::MakeShared<Aws::StringStream>("lambda argument", jsonPayload.View().WriteReadable());
    invokeRequest.SetBody(payload);
    invokeRequest.SetContentType("application/json");
    auto outcome = client.client->Invoke(invokeRequest);
    if (outcome.IsSuccess())
      return handleSuccessfulInvocation(outcome.GetResult());
    Aws::Lambda::LambdaError e = outcome.GetError();
    throw std::runtime_error(e.GetMessage());
  }
  EnhancedLambdaClient &client;
  std::string name;
};
}
#endif
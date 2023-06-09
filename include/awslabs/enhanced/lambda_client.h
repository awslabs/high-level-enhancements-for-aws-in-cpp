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
#include <iterator>
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

namespace alpaca::detail {
template<typename ...Ts>
struct aggregate_arity<std::tuple<Ts...>> : std::make_index_sequence<sizeof...(Ts)> {};
}

namespace AwsLabs::Enhanced {
struct EnhancedLambdaClient;

template<typename Sig>
struct Lambda;

struct EnhancedLambdaClient {
  EnhancedLambdaClient(Aws::Client::ClientConfiguration config = {}) {
    // Kludge that avoids silent poor concurrency. We want the default maximum
    // concurrency to be lambda's default concurrency of 1000. If we
    // see the built-in default of 25, we change it to 1000 on the reasonable
    // assumption that it got the value by default rather than explicitly.
    // Will create a more completely correct solution in the future.
    if(config.maxConnections == 25) config.maxConnections = 1000;
    client = std::make_unique<Aws::Lambda::LambdaClient>(config);
  }
  template<typename Sig>
  Lambda<Sig> bind_lambda(std::string name) {
    return Lambda<Sig>(*this, name);
  }
  std::unique_ptr <Aws::Lambda::LambdaClient> client;
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

template<typename Sig> struct Lambda;

template<typename R, typename ...Args>
struct Lambda<R(Args...)> {
  Lambda(EnhancedLambdaClient &client, std::string name)
      : client(client), name(name) {}

  static R handleSuccessfulInvocation(Aws::Lambda::Model::InvokeResult &result) {
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

  static expns::expected <R, std::string> outcomeToExpected(Aws::Lambda::Model::InvokeOutcome &outcome) {
    if(outcome.IsSuccess()) 
      return handleSuccessfulInvocation(outcome.GetResult());
    else
      return expns::unexpected(outcome.GetError().GetMessage());
  }

  R operator()(Args... args) {
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetFunctionName(name);
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
    // Add dummy args to work around https://github.com/p-ranav/alpaca/issues/22#issuecomment-1569568081 
    Detail::ArgsHolder argsHolder(std::tuple(0, 0, args...));
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

  template<typename Callable>
  void invoke_async(Callable c, Args... args) {
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetFunctionName(name);
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
    // Add dummy args to work around https://github.com/p-ranav/alpaca/issues/22#issuecomment-1569568081 
    Detail::ArgsHolder argsHolder(std::tuple(0, 0, args...));
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
    client.client->InvokeAsync(
      invokeRequest, 
      [c](const Aws::Lambda::LambdaClient*, const Aws::Lambda::Model::InvokeRequest&, 
          Aws::Lambda::Model::InvokeOutcome outcome, 
          const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) {
        c(outcomeToExpected(outcome));
      });
  }

  EnhancedLambdaClient &client;
  std::string name;
};


// In process
enum class cloud_launch {
  cloud
};
template<typename R, typename ...Args>
auto
async(cloud_launch, Lambda<R(Args...)> l, Args ...args) {
    auto p=std::make_shared<std::promise<R>>();
    auto f = p->get_future();
    l.invoke_async([p](expns::expected<R, std::string> e) { 
      try {
        p->set_value(e.value());
      } catch (...) {
        try {
          p->set_exception(std::make_exception_ptr(std::runtime_error(e.error())));
        } catch(...) {}
      }}, args...);
    return f;
}

template<typename InpIt, typename OutIt, typename R, typename ...Args>
auto
transform(cloud_launch, InpIt beg, InpIt end, OutIt out, Lambda<R(Args...)> l)
{
  std::vector<std::future<R>> futures;
  std::transform(beg, end, std::back_inserter(futures), [&](auto x) { 
    return async(cloud_launch::cloud, l, x); 
    });
  return std::transform(futures.begin(), futures.end(), out, [](auto &x) { return x.get(); });
}
}

#endif
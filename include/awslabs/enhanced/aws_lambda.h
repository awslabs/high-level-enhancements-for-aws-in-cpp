/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/HashingUtils.h>
#include <functional>
#include <stdexcept>
#ifdef __cpp_lib_expected
#include <expected>
namespace expns = std;
#else
#include <tl/expected.hpp>
namespace expns = tl;
#endif
#include <string>
#include <utility>
#include <sstream>
#include <alpaca/alpaca.h>

using namespace aws::lambda_runtime;
using namespace std::string_literals;
using Aws::Utils::Json::JsonValue;
#include "detail/lambda_detail.h"

namespace AwsLabs::Enhanced {
template <typename Func>
struct make_function
{
  using type = decltype(std::function(std::declval<Func>()));
};

template <typename R, typename... Ts>
struct make_function<std::function<R(Ts...)>>
{
  using type = std::function<R(Ts...)>;
};

template <typename Func>
using make_function_t = typename make_function<Func>::type;

namespace Detail {
  template <typename R>
  invocation_response makeResponse(R const &r)
  {
    JsonValue result;
    putValue(result, r);
    return invocation_response::success(result.View().WriteReadable(), "application/json");
  }

  invocation_response makeResponse(invocation_response const &ir)
  {
    return ir;
  }

  template<typename R>
  invocation_response makeBase64Response(R const &r)
  {
    Detail::ResultHolder<R> resultHolder(r);
    JsonValue result;
    std::vector<uint8_t> bytes;
    auto bytes_written = alpaca::serialize(resultHolder, bytes);
    static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Platform breaks our assumption that characters are 8 bits");
    Aws::Utils::ByteBuffer byteBuffer(reinterpret_cast<unsigned char *>(&bytes.front()), bytes_written);
    result.WithString("value",  Aws::Utils::HashingUtils::Base64Encode(byteBuffer));
    return invocation_response::success(result.View().WriteReadable(), "application/json");
  }

  template <typename R, typename... Args>
  invocation_response call(std::function<R(Args...)> const &f,
                           invocation_request const &req)
  {
    static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Platform breaks our assumption that characters are 8 bits");
    JsonValue v = req.payload;
    Aws::Utils::ByteBuffer bb = Aws::Utils::HashingUtils::Base64Decode(v.View().GetString("serialized"));
    std::vector<uint8_t> bytes(reinterpret_cast<uint8_t *>(bb.GetUnderlyingData()),
                               reinterpret_cast<uint8_t *>(bb.GetUnderlyingData() + bb.GetLength()));
    std::error_code ec;
    auto argsHolder = alpaca::deserialize<Detail::ArgsHolder<Args...>>(bytes, ec);
    if(ec) 
      throw std::runtime_error("Deserialization error code: " + ec.message());


    return makeBase64Response(std::apply(f, argsHolder.tup));
  };

  template <typename R>
  invocation_response call(std::function<R(JsonValue const &)> const &f,
                           invocation_request const &req)
  {
    return makeResponse(f(req.payload));
  };

  template <typename R>
  invocation_response call(std::function<R(invocation_request const &)> const &f,
                           invocation_request const &req)
  {
    return makeResponse(f(req));
  };

  template <typename Func>
  invocation_response respond(Func const &f, invocation_request const &req)
  {
    try
    {
      return call(f, req);
    }
    catch (std::exception const &exc)
    {
      return invocation_response::failure(exc.what(), "application/json");
    }
  }

  template <typename R, typename... Args>
  invocation_response respond(std::function<expns::expected<R, std::string>(Args...)> const &f,
                              invocation_request const &req)
  {
    auto result = f(req.payload);
    return result ? invocation_response::success(*result,
                                                 "application/json")
                  : invocation_response::failure(result
                                                     .error(),
                                                 "application/json");
  }

}

template <typename Func>
struct Handler
{
  template <typename H>
  Handler(H h) : func(h)
  {
    run_handler(*this);
  }

  invocation_response operator()(invocation_request const &req)
  {
    return Detail::respond(func, req);
  }
  Func func;
};

template <typename Func>
Handler(Func) -> Handler<make_function_t<Func>>;
}

int main()
{
  return 0;
}

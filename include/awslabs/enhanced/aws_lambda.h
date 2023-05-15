/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
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
using namespace aws::lambda_runtime;
using namespace std::string_literals;
using Aws::Utils::Json::JsonValue;

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

namespace detail
{
  template <typename T>
  T getArg(JsonValue, size_t idx);

  template <>
  bool getArg<bool>(JsonValue v, size_t idx)
  {
    return v.View().GetBool("key"s + std::to_string(idx));
  }

  template <>
  double getArg<double>(JsonValue v, size_t idx)
  {
    return v.View().GetDouble("key"s + std::to_string(idx));
  }

  template <>
  int64_t getArg<int64_t>(JsonValue v, size_t idx)
  {
    return v.View().GetInt64("key"s + std::to_string(idx));
  }

  template <>
  int getArg<int>(JsonValue v, size_t idx)
  {
    return v.View().GetInteger("key"s + std::to_string(idx));
  }

  template <>
  std::string getArg<std::string>(JsonValue v, size_t idx)
  {
    return v.View().GetString("key"s + std::to_string(idx));
  }

  template <size_t... I, typename R, typename... Args>
  R call_helper(std::index_sequence<I...>,
                std::function<R(Args...)> const &f, invocation_request const &req)
  {
    JsonValue v = req.payload;
    return f(getArg<Args>(v, I)...);
  };

  void putValue(JsonValue &v, bool b)
  {
    v.WithBool("value", b);
  }

  void putValue(JsonValue &v, double d)
  {
    v.WithDouble("value", d);
  }

  void putValue(JsonValue &v, int64_t i)
  {
    v.WithInt64("value", i);
  }

  void putValue(JsonValue &v, int i)
  {
    v.WithInteger("value", i);
  }

  void putValue(JsonValue &v, std::string s)
  {
    v.WithString("value", s);
  }

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

  template <typename R, typename... Args>
  invocation_response call(std::function<R(Args...)> const &f,
                           invocation_request const &req)
  {
    JsonValue result;
    return makeResponse(call_helper(std::make_index_sequence<sizeof...(Args)>(), f, req));
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
    return detail::respond(func, req);
  }
  Func func;
};

template <typename Func>
Handler(Func) -> Handler<make_function_t<Func>>;

int main()
{
  return 0;
}

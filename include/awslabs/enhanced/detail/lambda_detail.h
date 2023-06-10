#pragma once

namespace AwsLabs::Enhanced::Detail {
template<typename ...Ts>
struct ArgsHolder {
   std::tuple<Ts...> tup;

    ArgsHolder() = default;
    explicit ArgsHolder(const std::tuple<Ts...> &tup) : tup(tup) {}
};

template<typename T>
struct ResultHolder {
   T result;

    ResultHolder() = default;
    explicit ResultHolder(T const &result) : result(result) {}
};

}
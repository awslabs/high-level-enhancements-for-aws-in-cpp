#pragma once

namespace AwsLabs::Enhanced::Detail {
template<typename ...Ts>
struct ArgsHolder {
   std::tuple<Ts...> tup;
};

template<typename T>
struct ResultHolder {
   T result;
};

}
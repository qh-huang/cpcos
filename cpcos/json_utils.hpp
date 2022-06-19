#pragma once

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace cpcos {

struct Person
{
  string name;
  string address;
  uint32_t age;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, name, address, age)

}  // namespace cpcos

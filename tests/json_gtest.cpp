#include <gtest/gtest.h>

#include "json_utils.hpp"

TEST(JsonGTest, JsonGTest)
{
  cpcos::Person p = {"Ned Flanders", "744 Evergreen Terrace", 60};

  // serialization: person -> json
  json j = p;
  std::cout << "serialization: " << j << std::endl;

  // deserialization: json -> person
  json j2 = R"({"address": "742 Evergreen Terrace", "age": 40, "name": "Homer Simpson"})"_json;
  auto p2 = j2.get<cpcos::Person>();
  EXPECT_EQ(p2.name, "Homer Simpson");
  EXPECT_EQ(p2.address, "742 Evergreen Terrace");
  EXPECT_EQ(p2.age, 40);

  // incomplete deserialization:
  json j3 = R"({"address": "742 Evergreen Terrace", "name": "Maggie Simpson"})"_json;
  // try
  // {
  //     auto p3 = j3.get<cpcos::Person>();
  // }
  // catch (json::exception& e)
  // {
  //     std::cout << "deserialization failed: " << e.what() << std::endl;
  // }
  EXPECT_THROW(j3.get<cpcos::Person>(), json::exception);
}

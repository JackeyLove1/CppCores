#include <glog/logging.h>
#include <gtest/gtest.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <sstream>

#include "config.h"

class ConfigTest : public ::testing::Test {
public:
    void SetUp() {}

    void TearDown() {}
};

TEST_F(ConfigTest, TestLexicalCast) {
    try {
        auto value1 = 12345;
        std::string s1 = std::to_string(value1);
        auto value2 = boost::lexical_cast<int>(s1);
        ASSERT_EQ(value1, value2);
    } catch (const boost::bad_lexical_cast &err) {
        LOG(ERROR) << "Failed to lexical_cast, error: " << err.what();
    }
}

TEST_F(ConfigTest, TestYAMLSimple) {
    std::string yaml_content = R"(
    name: Jacky
    age: 99
    height: 200
    )";
    YAML::Node node = YAML::Load(yaml_content);
    for (size_t i = 0; i < node.size(); ++i) {
        std::stringstream ss;
        ss.str("");
        ss << node[i];
        LOG(INFO) << "index: " << i << " value:" << ss.str();
    }
}

TEST_F(ConfigTest, TestYAMLLoad) {
    std::string yaml_content = R"(
    name: Jacky
    age: 99
    address:
      city: Amsterdam
      country: Netherlands
    hobbies:
      - Coding
      - Reading
      - Running
    )";
    YAML::Node data = YAML::Load(yaml_content);
    auto name = data["name"].as<std::string>();
    EXPECT_EQ(name, "Jacky") << "Failed to parse name";
    int age = data["age"].as<int>();
    EXPECT_EQ(age, 99) << "Failed to parse age";
    auto city = data["address"]["city"].as<std::string>();
    auto country = data["address"]["country"].as<std::string>();
    EXPECT_EQ(city, "Amsterdam");
    EXPECT_EQ(country, "Netherlands");
    std::vector<std::string> hobbies;
    auto hobbies_node = data["hobbies"];
    for (auto iter = hobbies_node.begin(); iter != hobbies_node.end(); ++iter) {
        hobbies.emplace_back(iter->as<std::string>());
    }
    EXPECT_EQ(hobbies.size(), 3u);
}
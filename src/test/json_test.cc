#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "utils/json_util.h"

class JsonUtilTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

};

TEST_F(JsonUtilTest, BasicTest) {
    std::string json_data = R"(
    {
        "name": "John Doe",
        "age": 30,
        "is_member": true,
        "phones": [
            12345,
            67890
        ]
    })";
    Json::CharReaderBuilder rbuilder{};
    rbuilder["collectComments"] = false;
    std::string errs;
    Json::Value parsedFromString;
    std::istringstream sstr(json_data);
    bool parsingSuccessful = Json::parseFromStream(rbuilder, sstr, &parsedFromString, &errs);
    ASSERT_TRUE(parsingSuccessful);

    std::string name = parsedFromString["name"].asString();
    ASSERT_EQ(name, "John Doe");
    int age = parsedFromString["age"].asInt();
    ASSERT_EQ(age, 30);
    bool is_member = parsedFromString["is_member"].asBool();
    ASSERT_EQ(is_member, true);

    Json::Reader reader{};
    Json::Value parsedValue{};
    reader.parse(json_data, parsedValue);
    name = parsedValue["name"].asString();
    ASSERT_EQ(name, "John Doe");
    age = parsedValue["age"].asInt();
    ASSERT_EQ(age, 30);
    is_member = parsedValue["is_member"].asBool();
    ASSERT_EQ(is_member, true);
}

TEST_F(JsonUtilTest, JsonParseTest) {
    std::string json_data = R"(
    {
        "name": "John Doe",
        "age": 30,
        "is_member": true,
        "numbers": [0,1,2,3,4,5,6,7,8,9],
        "height" : 1.23,
        "salary":  -100,
        "address":{
                "city" : "NewYork",
                "state" : "NY"
        }
    })";

    auto parsedValue = JsonUtil::Parse(json_data);
    std::string name = parsedValue["name"].asString();
    ASSERT_EQ(name, "John Doe");
    int age = parsedValue["age"].asInt();
    ASSERT_EQ(age, 30);
    bool is_member = parsedValue["is_member"].asBool();
    ASSERT_EQ(is_member, true);
    double height = parsedValue["height"].asDouble();
    ASSERT_EQ(height, 1.23);
    int salary = parsedValue["salary"].asInt();
    ASSERT_EQ(salary, -100);
    auto numbers = parsedValue["number"];
    auto address = parsedValue["address"];
    EXPECT_EQ(address["city"].asString(), "NewYork");
    EXPECT_EQ(address["state"].asString(), "NY");
}
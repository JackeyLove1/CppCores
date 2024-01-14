/**
 * @file config.h
 * @brief config module, parse yaml file
 * @author JackyFan
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string_view>

#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <glog/logging.h>

class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;

    explicit ConfigVarBase(std::string &&name, const std::string &description = "") : m_name(std::move(name)),
                                                                                      m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    virtual ~ConfigVarBase() {}

    inline std::string_view getName() const noexcept { return m_name; }

    inline std::string_view getDescription() const noexcept { return m_description; }

    virtual std::string toString() = 0;

    /**
     * @brief 从字符串初始化值
     */
    virtual bool fromString(const std::string &val) = 0;

    /**
     * @brief 返回配置参数值的类型名称
     */
    virtual std::string getTypeName() const = 0;

protected:
    std::string m_name;
    std::string_view m_description;
};

template<class From, class To>
class LexicalCast {
public:
    To operator()(const From &value) {
        return boost::lexical_cast<To>(value);
    }
};

template<typename T>
class LexicalCast<std::string, std::vector<T>> {
public:
    std::vector<T> operator()(const std::string &v) {
        YAML::Node nodes = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (auto &&node: nodes) {
            ss.str("");
            ss << node;
            vec.emplace_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T> &v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &element: v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(element)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string &v) {
        YAML::Node nodes = YAML::Load(v);
        typename std::set<T> s;
        std::stringstream ss;
        for (auto &&node: nodes) {
            ss.str("");
            ss << node;
            s.emplace_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return s;
    }
};

template<typename T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T> &s) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &element: s) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(element)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string &v) {
        YAML::Node nodes = YAML::Load(v);
        typename std::unordered_set<T> s;
        std::stringstream ss;
        for (auto &&node: nodes) {
            ss.str("");
            ss << node;
            s.emplace_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return s;
    }
};

template<typename T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T> &s) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &element: s) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(element)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string &v) {
        YAML::Node nodes = YAML::Load(v);
        typename std::map<std::string, T> m;
        std::stringstream ss;
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            ss.str("");
            ss << it->second;
            m.emplace({it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())});
        }
        return m;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T> &v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto &i: v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T,
        class FromStr = LexicalCast<std::string, T>,
        class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar>;
    using on_change_cb = std::function<void(const T &old_value, const T &new_value)>;

    // explicit  ConfigVar(const std::string)
};
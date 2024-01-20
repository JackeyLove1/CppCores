#include "str_utils.h"
#include "utils.h"

std::string GenRandomString(size_t len) {
    std::string result{};
    result.resize(len);
    for (size_t i = 0; i < len; ++i) {
        result[i] = kDefaultChars[GenRandom<uint32_t>() % kDefaultCharsLen];
    }
    return result;
}

std::string replace(const std::string &str1, char find, char replaceWith) {
    auto str = str1;
    size_t index = str.find(find);
    while (index != std::string::npos) {
        str[index] = replaceWith;
        index = str.find(find, index + 1);
    }
    return str;
}

std::string replace(const std::string &str1, char find, const std::string &replaceWith) {
    auto str = str1;
    size_t index = str.find(find);
    while (index != std::string::npos) {
        str = str.substr(0, index) + replaceWith + str.substr(index + 1);
        index = str.find(find, index + replaceWith.size());
    }
    return str;
}

std::string replace(const std::string &str1, const std::string &find, const std::string &replaceWith) {
    auto str = str1;
    size_t index = str.find(find);
    while (index != std::string::npos) {
        str = str.substr(0, index) + replaceWith + str.substr(index + find.size());
        index = str.find(find, index + replaceWith.size());
    }
    return str;
}

std::vector<std::string> split(const std::string &str, char delim, size_t max) {
    std::vector<std::string> result;
    if (str.empty()) {
        return result;
    }

    size_t last = 0;
    size_t pos = str.find(delim);
    while (pos != std::string::npos) {
        result.push_back(str.substr(last, pos - last));
        last = pos + 1;
        if (--max == 1)
            break;
        pos = str.find(delim, last);
    }
    result.push_back(str.substr(last));
    return result;
}

std::vector<std::string> split(const std::string &str, const char *delims, size_t max) {
    std::vector<std::string> result;
    if (str.empty()) {
        return result;
    }

    size_t last = 0;
    size_t pos = str.find_first_of(delims);
    while (pos != std::string::npos) {
        result.push_back(str.substr(last, pos - last));
        last = pos + 1;
        if (--max == 1)
            break;
        pos = str.find_first_of(delims, last);
    }
    result.push_back(str.substr(last));
    return result;
}
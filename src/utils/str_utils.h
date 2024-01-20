#pragma once

#include <string>
#include <string_view>
#include "utils.h"

static constexpr std::string_view kDefaultChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr size_t kDefaultCharsLen = kDefaultChars.size();

std::string GenRandomString(size_t len);

std::string replace(const std::string &str, char find, char replaceWith);

std::string replace(const std::string &str, char find, const std::string &replaceWith);

std::string replace(const std::string &str, const std::string &find, const std::string &replaceWith);

std::vector<std::string> split(const std::string &str, char delim, size_t max = ~0);

std::vector<std::string> split(const std::string &str, const char *delims, size_t max = ~0);
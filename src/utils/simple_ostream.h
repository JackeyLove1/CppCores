#pragma once

#include <iostream>
#include <ostream>
#include <fstream>

template<typename T, typename std::enable_if_t<
        std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string>, void *> = nullptr>
bool EncodeToStream(std::ostream &os, T const &value) {
    os.write(reinterpret_cast<const char *>(&value), sizeof(T));
    return os.good();
}

template<typename T, typename std::enable_if_t<std::is_same_v<T, std::string>, void *> = nullptr>
bool EncodeToStream(std::ostream &os, const std::string &value) {
    EncodeToStream(os, value.size());
    os.write(reinterpret_cast<const char *>(value.data()), value.size());
    return os.good();
}

template<typename T, typename ... Args>
bool EncodeToStream(std::ostream &os, T const &t, Args const &... args) {
    if constexpr (sizeof ... (Args) > 0) {
        return EncodeToStream(os, t) && EncodeToStream(os, args ...);
    } else {
        return EncodeToStream(os, t);
    }
}

template<typename T, typename std::enable_if_t<
        std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string>, void *> = nullptr>
bool DecodeFromStream(std::istream &is, T *value) {
    is.read(reinterpret_cast<char *>(value), sizeof(T));
    return is.good();
}

template<typename T, typename std::enable_if_t<std::is_same_v<T, std::string>, void *> = nullptr>
bool DecodeFromStream(std::istream &is, std::string *value) {
    std::string::size_type size;
    DecodeFromStream(is, &size);
    value->resize(size);
    is.read(reinterpret_cast<char *>(value->data()), size);
    return is.good();
}

template<typename T, typename ... Args>
bool DecodeFromStream(std::istream &is, T *t, Args *... args) {
    if constexpr (sizeof ... (Args) > 0) {
        return DecodeFromStream(is, t) && DecodeFromStream(is, args ...);
    } else {
        return DecodeFromStream(is, t);
    }
}
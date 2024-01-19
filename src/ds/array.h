#pragma once

#include <cstdlib>
#include <iterator>
#include <memory>
#include <cstring>
#include <algorithm>
#include <functional>

template<typename T>
class Array {
public:
    // types
    typedef std::shared_ptr<Array> ptr;
    typedef T &reference;
    typedef const T &const_reference;
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit Array(const std::size_t N = 0) : size_{N}, data_{nullptr} {
        if (size_ > 0) {
            data_.reset(new T[size_]);
        }
    }

    explicit Array(const T *data, const uint64_t N) : size_{N} {
        data_.reset(new T[size_]);
        std::memcpy(data_.get(), data, size_ * sizeof(T));
    }

    Array(const Array &other) : size_{other.size_}, data_{new T[other.size_]} {
        std::copy(other.begin(), other.end(), data());
    }

    Array(Array &&other) noexcept: size_{other.size_}, data_{std::move(other.data_)} {}

    ~Array() {}

    // iterators
    iterator begin() noexcept { return iterator(data()); };

    iterator end() noexcept { return iterator(data() + size_); };

    const_iterator begin() const noexcept { return const_iterator(data()); };

    const_iterator end() const noexcept { return const_iterator(data() + size_); };

    // capacity
    constexpr size_t size() const noexcept { return size_; };

    // element access
    reference operator[](size_t n) {
        AssertIndex(n);
        return data_[n];
    };

    reference at(size_t n) {
        AssertIndex(n);
        return data_[n];
    };

    const_reference at(size_t n) const {
        AssertIndex(n);
        return data_[n];
    };


    T *data() noexcept { return data_.get(); };

    const T *data() const noexcept { return data_.get(); };

    bool is_sorted() const {
        return std::is_sorted(begin(), end());
    }

    bool is_sorted(std::function<bool(const T &, const T &)> cmp) const {
        return std::is_sorted(begin(), end(), cmp);
    }

    void sort() {
        std::sort(begin(), end());
    }

    bool insert(uint64_t pos, T &&val) {
        AssertIndex(pos);
        auto tmp = std::move(data_);
        data_.reset(new T[size_ + 1]);
        std::move(tmp.begin(), tmp.begin() + pos, data());
        data_[pos] = val;
        std::move(tmp.begin() + pos, tmp.end(), data() + pos + 1);
    }

protected:
    inline void AssertIndex(size_t n) {
        if (n >= size_) throw std::out_of_range("Array::at");
    }

private:
    uint64_t size_{};
    std::unique_ptr<T[]> data_;
};

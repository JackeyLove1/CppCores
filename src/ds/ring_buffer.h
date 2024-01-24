#include <algorithm>
#include <iterator>
#include <vector>

// TODO(Jacky): improve it with Linux kfifo
template<typename T>
class RingBuffer {
public:
    std::vector<T> buffer_;
    size_t reader_, writer_, capacity_;

public:
    explicit RingBuffer(size_t size) : reader_{0}, writer_{0}, capacity_{size} {
        buffer_.resize(size);
    }


    typedef typename std::vector<T>::iterator vector_iterator;

    struct iterator : public vector_iterator {
        RingBuffer *ring_;
        vector_iterator iter_;

        explicit iterator(RingBuffer *ring, vector_iterator iter) :
                iter_{iter}, ring_{ring} {}

        inline iterator &operator++() noexcept {
            if (++iter_ == ring_->buffer_.end()) {
                iter_ = ring_->buffer_.begin();
            }
            return *this;
        }

        inline iterator operator++(int) noexcept {
            auto ret = *this;
            ++*this;
            return ret;
        }

        inline iterator &operator--() noexcept {
            if (iter_ == ring_->buffer_.begin()) {
                iter_ = ring_->buffer_.end();
            } else {
                --iter_;
            }
            return *this;
        }

        inline iterator operator--(int) noexcept {
            auto ret = *this;
            --*this;
            return ret;
        }
    };

    iterator begin() noexcept {
        return iterator(this, buffer_.begin());
    }

    iterator end() noexcept {
        return iterator(this, buffer_.end());
    }

    void resize(size_t new_capacity) {
        if (new_capacity < capacity_) {
            return;
        }

        std::vector<T> new_buffer;
        new_buffer.resize(new_capacity);
        size_t s = writer_ - reader_;
        if (writer_ >= reader_) {
            std::copy(buffer_.begin() + reader_, buffer_.end() + writer_, new_buffer.begin());
        } else {
            auto curr = std::copy(buffer_.begin() + reader_, buffer_.end(), new_buffer.begin());
            std::copy(buffer_.begin(), buffer_.begin() + writer_, curr);
        }
        reader_ = 0, writer_ = s;
        capacity_ = new_capacity;
        std::swap(buffer_, new_buffer);
    }

    size_t capacity() const noexcept { return capacity_; }

    size_t size() const noexcept {
        return (writer_ - reader_ >= 0) ? (writer_ - reader_) : (buffer_.size() - reader_ + writer_);
    }

    bool empty() const noexcept { return writer_ == reader_; }

    size_t next(size_t n) {
        return (n + 1) % buffer_.size();
    }

    size_t prev(size_t n) {
        return (n > 0) ? ((n - 1) % buffer_.size()) : (buffer_.size() - 1);
    }

    bool push_back(T &&value) {
        size_t s = next(writer_);
        if (s == reader_) {
            return false;
        }
        buffer_[writer_] = value;
        writer_ = s;
        return true;
    }

    T &front() {
        return buffer_[reader_];
    }

    T &back() {
        return buffer_[prev(writer_)];
    }

    void force_push(T &&value) {
        if (capacity_ == 0) {
            throw std::runtime_error("Capacity Zero can't force push!");
        }
        size_t s = next(writer_);
        if (s == reader_) {
            reader_ = next(reader_);
        }
        buffer_[writer_] = value;
    }
};
#include <algorithm>
#include <iterator>
#include <vector>

template<typename T>
class RingBuffer {
public:
    std::vector<T> buffer_;
    size_t reader_, writer_, capacity_;

public:
    explicit RingBuffer(size_t size):reader_{0}, writer_{0}, capacity_{size}{
        buffer_.resize(size);
    }



};
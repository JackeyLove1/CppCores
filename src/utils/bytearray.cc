#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "bytearray.h"
#include "macro.h"

ByteArray::Node::Node(size_t s)
        : ptr{new char[s]},
          next{nullptr},
          size{s} {}

ByteArray::Node::Node()
        : ptr{nullptr},
          next{nullptr},
          size{0} {}

ByteArray::Node::~Node() {
    if (ptr) {
        delete[] ptr;
    }
}

ByteArray::ByteArray(size_t base_size)
        : m_baseSize{base_size},
          m_position{0},
          m_capacity{base_size},
          m_size{0},
          m_endian{BIG_ENDIAN},
          m_root{new Node(base_size)},
          m_cur{m_root} {}

ByteArray::~ByteArray() {
    auto *tmp = m_root;
    while (tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

bool ByteArray::isLittleEndian() const { return m_endian == LITTLE_ENDIAN; }

void ByteArray::clear() {
    m_position = 0;
    m_capacity = m_baseSize;
    Node *tmp = m_root->next;
    while (tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr;
}

void ByteArray::addCapacity(size_t size) {
    if (size == 0) {
        return;
    }
    size_t old_cap = getCapacity();
    if (old_cap >= size) {
        return;
    }

    size = size - old_cap;
    size_t count = ceil(static_cast<double>(size) / static_cast<double>(m_baseSize));
    Node *tmp = m_root;
    while (tmp->next) {
        tmp = tmp->next;
    }

    Node *first = nullptr;
    for (size_t i = 0; i < count; ++i) {
        tmp->next = new Node(m_baseSize);
        if (first == nullptr) {
            first = tmp->next;
        }
        tmp = tmp->next;
        m_capacity += m_baseSize;
    }
    if (old_cap == 0) {
        m_cur = first;
    }
}

void ByteArray::write(const void *buf, size_t size) {
    if (size == 0) {
        return;
    }
    addCapacity(size);

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;

    while (size > 0) {
        if (ncap >= size) {
            std::memcpy(m_cur->ptr + npos, static_cast<const char *>(buf) + bpos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        } else {
            std::memcpy(m_cur->ptr + npos, static_cast<const char *>(buf) + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }

    if (m_position > m_size) {
        m_size = m_position;
    }
}

void ByteArray::read(void *buf, size_t size) {
    if (size > getReadSize()) {
        throw std::out_of_range("not enough length");
    }

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size > 0) {
        if (ncap >= size) {
            std::memcpy(static_cast<char *>(buf) + bpos, m_cur->ptr + npos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        } else {
            std::memcpy(static_cast<char * >(buf) + bpos, m_cur->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
}
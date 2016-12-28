//
// Created by spooky on 2016-03-09.
//

#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H

#include <array>
// #include <glob.h>
// #include <iostream>

namespace db {

/**
 * Little Endian
 *
 * 10110011_10001111_00001111_10000011_11110000_00111111_10000000_11111111
 * ^ least significant                                  most significant ^
 * >------------------------------->
 */
class BaseBuffer {
protected:
  size_t m_position{0};
  size_t m_size{0};

private:
public:
  BaseBuffer() {
  }
  BaseBuffer(const BaseBuffer &o) : m_position(o.m_position), m_size(o.m_size) {
  }
  virtual ~BaseBuffer() {
  }
};

template <size_t t_capacity>
class Buffer : public BaseBuffer {
private:
  using Arr = std::array<uint8_t, t_capacity>;
  Arr m_data;

public:
  explicit Buffer(Arr &&data) : m_data(std::move(data)) {
  }

  Buffer() : Buffer(Arr{0}) {
  }

  Buffer(const Buffer &) = delete;

  Buffer(Buffer &&o) : BaseBuffer(o), m_data(std::move(o.m_data)) {
  }

private:
  void i_put(uint8_t datum) {
    m_data[m_position++] = datum;
    ++m_size;
  }

  template <size_t bytes>
  void i_get(std::array<uint8_t, bytes> &buff) {
    for (size_t i(0); i < buff.size(); ++i) {
      buff[i] = m_data[m_position++];
    }
  }

public:
  size_t postion() const {
    return m_position;
  }

  size_t size() const {
    return m_size;
  }

  void flip() {
    m_position = size_t(0);
  }

  void clear() {
    m_position = size_t(0);
    m_size = size_t(0);
  }

  void put(uint8_t datum) {
    if (postion() >= capacity()) {
      throw std::runtime_error("put position() >= capacity()");
    }
    i_put(datum);
  }

  void put(const uint8_t *datum, size_t length) {
    if (postion() + length >= capacity()) {
      throw std::runtime_error("put position() + length >= capacity()");
    }
    auto begin = &datum[0];
    auto end = begin + length;
    for (auto it = begin; it != end; it++) {
      i_put(*it);
    }
  }

  template <size_t size>
  void put(const uint8_t (&datum)[size]) {
    put(datum, size);
  }

  template <size_t size>
  void put(const std::array<uint8_t, size> &datum) {
    put(&datum.front(), size);
  }

  //        void put(uint32_t datum) {
  //
  //        }

  uint8_t get() {
    if (postion() > size()) {
      throw std::runtime_error("get position() > size()");
    }
    return m_data[m_position++];
  }

  template <size_t bytes>
  void get(std::array<uint8_t, bytes> &buff) {
    if ((postion() + bytes) > size()) {
      throw std::runtime_error("get position() + bytes > size()");
    }
    i_get(buff);
  }

  uint8_t *writable_data() {
    return m_data.data() + postion();
  }

  const uint8_t *full_data() const {
    return m_data.data();
  }

  void position(size_t pos) {
    m_position = pos;
    if (postion() > size()) {
      m_size = pos;
    }
  }

  constexpr size_t capacity() const {
    return t_capacity;
  }
};

class HeapBuffer {
public:
  // HeapBuffer(){
  // }
};
}

#endif // PROJECT_BUFFER_H

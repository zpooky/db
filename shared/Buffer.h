#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H

#include <array>
#include <iostream>
#include <sstream>

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
  size_t m_position;
  size_t m_size;
  const size_t m_capacity;

private:
public:
  BaseBuffer(size_t p_position, size_t p_size, size_t p_capacity)
      : m_position(p_position), m_size(p_size), m_capacity(p_capacity) {
  }
  BaseBuffer() : BaseBuffer(0, 0, 0) {
  }
  BaseBuffer(const BaseBuffer &o)
      : BaseBuffer(o.m_position, o.m_size, o.m_capacity) {
  }
  virtual ~BaseBuffer() {
  }

protected:
  virtual void i_put(uint8_t) = 0;
  virtual void i_get(uint8_t *, size_t) = 0;

  template <size_t bytes>
  void i_get(std::array<uint8_t, bytes> &buff) {
    i_get(buff.data(), buff.size());
  }

  virtual const uint8_t *data() const = 0;
  virtual uint8_t *data() = 0;

public:
  size_t capacity() const {
    return m_capacity;
  }
  size_t position() const {
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
    // TODO zero memory
  }

public:
  void put(uint8_t datum) {
    if (position() >= capacity()) {
      throw std::runtime_error("put position() >= capacity()");
    }
    i_put(datum);
  }

  void put(const uint8_t *datum, size_t length) {
    if (position() + length >= capacity()) {
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

  uint8_t get() {
    if (position() > size()) {
      throw std::runtime_error("get position() > size()");
    }
    return data()[m_position++];
  }

  template <size_t bytes>
  void get(std::array<uint8_t, bytes> &buff) {
    if ((position() + bytes) > size()) {
      std::stringstream b("");
      b << "position(" << position() << ") + bytes(" << bytes << ") > size("
        << size() << ")capacity[" << capacity() << "]" << std::endl;
      throw std::runtime_error(b.str());
    }
    i_get(buff);
  }

  uint8_t *writable_data() {
    return data() + position();
  }

  const uint8_t *full_data() const {
    return data();
  }

  void position(size_t pos) {
    m_position = pos;
    if (position() > size()) {
      m_size = pos;
    }
  }
};

class PointerBuffer : public BaseBuffer {
private:
  uint8_t *m_data;

public:
  explicit PointerBuffer(uint8_t *ptr, uint64_t p_size)
      : BaseBuffer(0, 0, p_size), m_data(ptr) {
  }

protected:
  void i_put(uint8_t datum) {
    // std::cout<<"datum[c"<<m_capacity<<",s"<<m_size<<",p"<<m_position<<"]="<<datum<<"\n";
    m_data[m_position++] = datum;
    ++m_size;
  }

  void i_get(uint8_t *buff, size_t size) {
    for (size_t i(0); i < size; ++i) {
      buff[i] = m_data[m_position++];
    }
  }
  const uint8_t *data() const {
    return m_data;
  }
  uint8_t *data() {
    return m_data;
  }
};

template <size_t t_capacity>
class Buffer : public BaseBuffer {
private:
  using Arr = std::array<uint8_t, t_capacity>;
  Arr m_data;

public:
  explicit Buffer(Arr &&data)
      : BaseBuffer(0, 0, t_capacity), m_data(std::move(data)) {
  }

  Buffer() : BaseBuffer(0, 0, t_capacity) {
  }

  Buffer(const Buffer &) = delete;

  Buffer(Buffer &&o) : BaseBuffer(o), m_data(std::move(o.m_data)) {
  }

protected:
  void i_put(uint8_t datum) {
    m_data[m_position++] = datum;
    ++m_size;
  }

  void i_get(uint8_t *buff, size_t size) {
    for (size_t i(0); i < size; ++i) {
      buff[i] = m_data[m_position++];
    }
  }

  const uint8_t *data() const {
    return m_data.data();
  }

  uint8_t *data() {
    return m_data.data();
  }

public:
};

class HeapBuffer {
public:
  // HeapBuffer(){
  // }
};
}

#endif // PROJECT_BUFFER_H

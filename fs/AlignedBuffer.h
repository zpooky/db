#ifndef _PROJECT_ALIGNED_BUFFER_H
#define _PROJECT_ALIGNED_BUFFER_H

#include <fcntl.h>
#include <utility>
#include <stdint.h>
#include <unistd.h>
#include <cassert>

namespace db {
struct AlignedBuffer {
private:
  uint8_t *m_alloc;
  uint8_t *m_aligned;

  size_t m_index = 0;
  size_t m_size = 0;
  size_t m_capacity;

public:
  /**
   * /malloc/ gives you a chunk of memory that could have any alignment.
   * /posix_memalign/ gives you a chunk of memory that is guaranteed
   * to have the requested alignment.
   *
   * So the result of posix_memalign(&p, 32, 128) will be a 128-byte
   * chunk of memory whose start address is guaranteed to be a multiple of 32.
   */
  explicit AlignedBuffer(size_t alignment)
      : AlignedBuffer(alignment, alignment) {
  }
  explicit AlignedBuffer(size_t alignment, size_t bytes)
      : m_capacity(size_t(sizeof(uint8_t)) * bytes) {
    assert(alignment > 0);
    assert(bytes > 0);
    assert(bytes <= alignment);
    assert(alignment % bytes == 0);
    /* Alloc dubble the size to ensure alignment fits
     */
    size_t alloc_size(2 * bytes);
    m_alloc = (uint8_t *)::calloc(alloc_size, sizeof(uint8_t));
    if (!m_alloc) {
      throw std::runtime_error("AlignedBuffer alloc");
    }
    /* Number offset bytes
     */
    size_t diff = ((size_t)m_alloc) % alignment;
    assert(alloc_size - diff >= bytes);
    /* Create a aligned pointer
     */
    m_aligned = m_alloc + diff;
    //::posix_memalign();
  }
  void flip() {
    std::swap(m_index, m_size);
  }
  void clear() {
    // TODO zero buffer
    m_index = m_size = 0;
  }
  size_t index() const {
    return m_index;
  }
  size_t size() const {
    return m_size;
  }
  size_t capacity() const {
    return m_capacity;
  }

  bool is_full() const {
    assert(size() <= capacity());
    return size() == capacity();
  }
  bool is_empty() const {
    return m_index == 0;
  }

  const uint8_t *full_data() const {
    return m_aligned;
  }

  ~AlignedBuffer() {
    if (m_alloc) {
      ::free(m_alloc);
      m_alloc = nullptr;
    }
  }
};
}
#endif

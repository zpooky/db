#ifndef DB_EXTENTS_BUILER_H
#define DB_EXTENTS_BUILER_H

#include "../shared/shared.h"
#include <bitset>
#include <cstddef>
#include <deque>

namespace db {

template <size_t capacity>
class ExtentBuilder {
private:
  using Bitset_t = std::bitset<capacity>;
  Bitset_t m_present;
  size_t m_current;

public:
  explicit ExtentBuilder() : m_present(0), m_current(0) {
  }

  ExtentBuilder(ExtentBuilder<capacity> &&o)
      : m_present{std::move(o.m_present)}, m_current{o.m_current} {
  }

  ExtentBuilder(const ExtentBuilder<capacity> &o)
      : m_present{o.m_present}, m_current{o.m_current} {
  }

  void next(bool present) {
    m_present[m_current++] = present;
  }

  bool is_full() const {
    return m_current == capacity;
  }

  size_t lines() const {
    return m_current;
  }

  const Bitset_t &present() const & {
    return m_present;
  }

  Bitset_t &present() & {
    return m_present;
  }

  Bitset_t &&present() && {
    return std::move(m_present);
  }
};

template <size_t lines>
class ExtentsBuilder {
private:
  using ExtentBuilder_t = ExtentBuilder<lines>;

private:
  std::deque<ExtentBuilder_t> m_extents;
  db::segment::id m_segment;
  ExtentBuilder_t *m_current;

public:
  explicit ExtentsBuilder(const db::segment::id &id = db::segment::NO_ID)
      : m_extents(), m_current(nullptr), m_segment(id) {
  }

private:
  bool is_full() const {
    return m_current == nullptr || m_current->is_full();
  }

public:
  void next(bool present) {
    if (is_full()) {
      m_extents.emplace_back();
      m_current = &m_extents.back();
    }
    m_current->next(present);
  }

  void swap(ExtentsBuilder<lines> &o) noexcept {
    std::swap(m_extents, o.m_extents);
    std::swap(m_segment, o.m_segment);
    std::swap(m_current, o.m_current);
  }

  const db::segment::id &segment() const {
    return m_segment;
  }

  const std::deque<ExtentBuilder_t> &builders() const & {
    return m_extents;
  }

  std::deque<ExtentBuilder_t> &builders() & {
    return m_extents;
  }

  std::deque<ExtentBuilder_t> &&builders() && {
    return std::move(m_extents);
  }
};

} // namespace db

#endif

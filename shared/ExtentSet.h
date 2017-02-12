#ifndef DB_EXTENT_SET_H
#define DB_EXTENT_SET_H

#include "../collection/CachedAllocator.h"
#include "../shared/Configuration.h"
#include "PresentSet.h"
#include "shared.h"
#include <algorithm>
#include <iostream>

namespace db {

template <typename T>
struct Lazy {
  T data;
  size_t refrences;

  Lazy(T &&d, size_t r = 1) : data(std::move(d)), refrences(r) {
  }

  Lazy(const Lazy &) = delete;
  Lazy(const Lazy &&) = delete;

  ~Lazy() {
  }

  template <typename A>
  bool operator<(const A &o) const {
    return data < o;
  }
  template <typename A>
  bool operator==(const A &o) const {
    return data == o;
  }

  bool is_valid() const {
    return refrences > 0;
  }

  void dec() {
    --refrences;
  }
  void inc() {
    ++refrences;
  }
};

template <typename T>
class LazyVector {
public:
  using AllocType = Lazy<T>;
  using It = AllocType **;

private:
  sp::CachedAllocator &m_allocator;
  It m_arr;
  size_t m_size;
  size_t m_capacity;

public:
  LazyVector(sp::CachedAllocator &a, size_t capacity, std::vector<T> &&v)
      : m_allocator(a), m_arr(nullptr), m_size(0),
        m_capacity(std::max(capacity, v.size())) {
    m_arr = new Lazy<T> *[m_capacity] { nullptr };
    for (auto &c : v) {
      insert(std::move(c));
    }
    for (size_t i(m_size); i < m_capacity; ++i) {
      m_arr[i] = nullptr;
    }
  }

  LazyVector(const LazyVector &) = delete;

  LazyVector(LazyVector &&o)
      : m_allocator(o.m_allocator), m_arr(nullptr), m_size(o.m_size),
        m_capacity(o.m_capacity) {
    std::swap(m_arr, o.m_arr);
  }

  template <typename S>
  It find(const S &value) const {
    // TODO verify. man std::lower_bound
    auto first = begin();
    auto last = end();

    auto comp = [](const AllocType *a, const auto &search) {
      //
      return a->data < search;
    };

    first = std::lower_bound(first, last, value, comp);
    return first != last && !((*first)->data == value) ? first : last;
  }

  template <typename K, typename Comp>
  void insertion_sort(K *arr, size_t &size, K &value, Comp lt) {
    size_t mid(size / 2);
    if (lt(arr[mid], value)) {

    }
    arr[size++] = value;
    // TODO sort
  }

  void insert(T &&c) {
    assert(m_size < m_capacity);
    // TODO raii?
    AllocType *ptr = m_allocator.allocate<AllocType>();
    new (ptr) AllocType(std::move(c));
    auto lt = [](const AllocType *f, const AllocType *s) {
      //
      return f->data < s->data;
    };
    insertion_sort(m_arr, m_size, ptr, lt);
  }

private:
  It begin() const {
    // TODO verify itterator
    return m_arr;
  }

public:
  It end() const {
    return m_arr + m_size;
  }

  ~LazyVector() {
    // std::cout << "~LazyVector|" << this << "|\n";
    if (m_arr) {
      // std::cout << "~LazyVector()" << m_size << "|" << m_capacity << "\n";
      for (size_t i(0); i < m_size; ++i) {
        auto cur = m_arr[i];
        // std::cout << "dec(" << it << ")" << i++ << "\n";
        cur->dec();
        if (!cur->is_valid()) {
          // std::cout << "deallocate(" << it << ")\n";
          // m_allocator.deallocate(it);
        }
      }
      delete[] m_arr;
      m_arr = nullptr;
    }
  }
};

class HeapExtentSet {
private:
  using PS = db::PresentSet<db::Configuration::extent_lines()>;
  using Vector_t = LazyVector<PS>;
  using Allocator = sp::CachedAllocator;

public:
  using AllocType = Vector_t::AllocType;

private:
  PageRange m_range;
  const db::segment::id m_segment;
  Allocator m_allocator;
  Vector_t m_extents;

public:
  HeapExtentSet(const db::segment::id &s, size_t max_extents, Allocator &&a,
                std::vector<PS> &&exts)
      : m_range(0, 0), m_segment(s), m_allocator(std::move(a)),
        m_extents{m_allocator, max_extents, std::move(exts)} {
    // TODO define range
  }

  HeapExtentSet(const HeapExtentSet &) = delete;

  HeapExtentSet(HeapExtentSet &&o)
      : m_range(o.m_range), m_segment(o.m_segment),
        m_allocator(std::move(o.m_allocator)), m_extents(std::move(m_extents)) {
  }

  ~HeapExtentSet() {
  }

  /**
   * introduce new extents
   */
  template <size_t capacity>
  void introduce(const db::PresentSet<capacity> &o) {
    m_extents.insert(o);
    auto size(m_range.size() + o.range().size());
    assert(size > m_range.size());
    m_range = PageRange(0, size);
  }

  bool operator[](page::position search) const {
    auto it = m_extents.find(search);
    if (it != m_extents.end()) {
      return (*it)->data.operator[](search);
    }
    return false;
  }
};
}

#endif

#ifndef _DB_SEGMENT_EXTENTS_H
#define _DB_SEGMENT_EXTENTS_H

#include "../collection/List.h"
#include "Extent.h"

namespace db {
template <typename Meta_t>
class Extents {
private:
  sp::con::Stack<Extent<Meta_t>> m_extents;

public:
  template <typename Collection>
  explicit Extents(Collection &&e) : m_extents(std::move(e)) {
  }
  auto reserve() {
    // TODO think of something better
    // algorithm instead of loops
    for (auto &e : m_extents) {
      auto maybe = e.reserve();
      if (maybe) {
        return maybe;
      }
    }
    return sp::Maybe<page::position>();
  }

  bool has_free() const {
    // TODO think of something better
    // algorithm instead of loops
    for (const auto &e : m_extents) {
      if (e.has_free()) {
        return true;
      }
    }
    return false;
  }
};
}
#endif

#ifndef DB_MAIN_UTIL_H
#define DB_MAIN_UTIL_H

#include "../page/io/FilePageMeta.h"
#include "../segment/Extent.h"
#include "../shared/PresentSet.h"
#include "ExtentsBuilder.h"
#include <algorithm>
#include <deque>
#include <vector>

namespace db {
namespace segment {

// TODO merge function with presents_sets...
template <typename Meta_t, size_t lines = Meta_t::extent_lines()>
std::vector<Extent<Meta_t>>
extents(const std::deque<ExtentBuilder<lines>> &builders) {
  std::vector<Extent<Meta_t>> extents;
  page::position start(0);
  for (auto &extent : builders) {
    auto extent_lines(extent.lines());
    db::PageRange range(start, extent_lines);
    extents.emplace_back(PresentSet<lines>(extent.present(), range));
    start = page::position(start + extent_lines);
  }
  return extents;
}

template <size_t lines>
std::vector<db::PresentSet<lines>>
present_sets(const std::deque<db::ExtentBuilder<lines>> &b) {
  std::vector<db::PresentSet<lines>> res;
  page::position start(0);
  auto t = [&](const auto &ext) {
    auto extent_lines(ext.lines());
    db::PageRange range(start, extent_lines);
    start = page::position(start + extent_lines);
    return db::PresentSet<lines>(ext.present(), range);
  };
  std::transform(b.begin(), b.end(), std::back_inserter(res), t);
  return res;
}
} // namespace segment
} // namespace db

#endif

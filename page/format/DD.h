#ifndef _DB_FORMAT_DD
#define _DB_FORMAT_DD

#include "../../shared/conversions.h"
#include "../../shared/entities.h"
#include "../../shared/shared.h"
#include <tuple>
#include <utility>
#include <vector>
#include <algorithm>

namespace page {
class DD {
private:
  const db::Directory &m_root;

public:
  explicit DD(const db::Directory &root) : m_root(root) {
  }

  DD(const DD &) = delete;
  DD(const DD &&) = delete;

  std::tuple<db::segment::id, std::vector<db::File>> operator()() {
    std::vector<db::File> segments = files(m_root);

    std::vector<db::segment::id> num_segments(segments.size());
    std::transform(segments.begin(), segments.end(), num_segments.begin(),
                   [](const auto &file) {
                     auto fname = file.filename();
                     return db::to<db::segment::id>(fname.name.c_str());
                   });

    auto max_it = std::max_element(num_segments.begin(), num_segments.end());
    auto max = max_it != num_segments.end() ? *max_it : db::segment::id(0);

    return std::make_tuple(max, segments);
  }
};
}
#endif

#ifndef _DB_CONFIGURATION_H
#define _DB_CONFIGURATION_H

#include "../shared/entities.h"

namespace db {
struct Configuration {
  /**
   * db root directory
   */
  const Directory root;
  /**
   * TODO make configurable
   * max extents per segments file
   */
  const size_t extents;

  explicit Configuration(const Directory &p_root) : root(p_root), extents(100) {
  }

  /**
   * lines per extent
   */
  static constexpr size_t extent_lines() {
    return 1024ul;
  }
};
}

#endif

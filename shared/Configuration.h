#ifndef _DB_CONFIGURATION_H
#define _DB_CONFIGURATION_H

#include "../shared/entities.h"

namespace db {
struct Configuration {
  const Directory root;

public:
  explicit Configuration(const Directory &p_root) : root(p_root) {
  }
};
}

#endif

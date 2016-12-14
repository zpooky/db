//
// Created by spooky on 2016-03-03.
//

#ifndef DB_FS_H
#define DB_FS_H

#include "../config/Configuration.h"
#include <array>
#include <stddef.h>
#include <string>
#include <utility>

namespace db {

template <typename Table_t>
struct Reservation {
  const db::segment::id segment;
  const size_t position;

  explicit Reservation() : Reservation(0, 0) {
    // TODO fix this
  }

  explicit Reservation(db::segment::id p_id, size_t p_position)
      :  segment{p_id}, position{p_position} {
  }
  bool is_valid() const {
    return position != size_t(0) || segment != db::segment::id(0);
  }
};

class Table {
  //        static size_t size() const noexcept = 0;
};

// Raw
using rid = uint64_t;
constexpr rid EMPTY_LINE = 0ul;
template <size_t size>
using raw = std::array<uint8_t, size>;
}
namespace journal {

using journal_id = uint64_t;
}

#endif // DB_FS_H

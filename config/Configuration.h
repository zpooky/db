//
// Created by spooky on 2016-03-03.
//

#ifndef DB_CONFIG_H
#define DB_CONFIG_H

#include <array>
#include <string>

namespace db {
namespace table {
namespace name {
constexpr size_t length = 64;
using type = std::array<char, length>;
}
}
namespace reservation {
using id = uint64_t;
}
namespace segment {
using version = uint16_t;
using id = uint64_t;
using file_id = uint64_t;
}
template <typename T_table>
struct Segment_name {
  Segment_name() {
  }

  static std::string name(segment::id id) {
    char buf[64];
    //            char buf[table::name::length + 1 + 32];
    //            auto table_name = T_table::table_name();
    //            string name{table_name.begin(), table_name.end()};
    //            sprintf(buf, "%s-%ld", name.c_str(), idx);

    ::sprintf(buf, "%lu", id);
    return std::string{buf};
  }
};
}

#endif

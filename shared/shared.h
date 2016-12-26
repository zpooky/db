#ifndef _H_SHARED_IDS
#define _H_SHARED_IDS

#include <array>
#include <string>

namespace journal {

using id = uint64_t;
constexpr id START_ID(1);
constexpr id NO_ID(0);
}
namespace page {

using position = uint64_t;
}
namespace db {

namespace table {
using version = uint64_t;

namespace name {

constexpr size_t length(64);
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
constexpr id NO_ID(0);
constexpr id START_ID(1);
}
namespace raw {

using id = uint64_t;
using version_t = uint64_t;
constexpr id EMPTY_LINE(0ul);
template <size_t size>
using type = std::array<uint8_t, size>;
}

namespace transaction {

using xid_t = uint64_t;
using id = uint64_t;
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

template <typename Table_t>
struct Reservation {
  const db::segment::id segment;
  const page::position position;

  explicit Reservation() : Reservation(0, 0) {
    // TODO fix this
  }

  explicit Reservation(db::segment::id p_id, size_t p_position)
      : segment{p_id}, position{p_position} {
  }
  bool is_valid() const {
    return position != size_t(0) || segment != db::segment::id(0);
  }
};

class Table {
  //        static size_t size() const noexcept = 0;
};
}
#endif

#ifndef _H_SHARED_IDS
#define _H_SHARED_IDS

#include "conversions.h"
#include "entities.h"
#include <array>
#include <string>

// #define DB_ID_CLASS() \

namespace journal {
struct idx {
  const uint64_t _id;
  explicit idx(uint64_t p_id) : _id(p_id) {
  }
  bool operator==(const idx &o) const {
    return _id == o._id;
  }
  idx operator++() const {
    return idx(_id + 1);
  }
  idx operator++(int)const {
    return idx(_id + 1);
  }
};

using id = uint64_t;
constexpr id START_ID(1);
constexpr id NO_ID(0);
} // namespace journal

namespace tx {

using id = uint64_t;
constexpr id START_ID(1);
} // namespace tx

namespace page {

using position = uint64_t;
} // namespace page

namespace db {

namespace table {
using id = uint64_t;
using version = uint64_t;

namespace name {

constexpr size_t length(64);
using type = std::array<char, length>;
} // namespace name
} // namespace table

namespace segment {

// using file_id = uint64_t;

using version = uint16_t;

using id = uint64_t;
constexpr id NO_ID(0);
constexpr id START_ID(1);
} // namespace segment
namespace raw {

using size = uint64_t;

using id = uint64_t;
constexpr id START_ID(1);
constexpr id EMPTY(0ul);

using version_t = uint64_t;
constexpr version_t START_VERSION(1);
} // namespace raw

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

  static segment::id id(const db::Filename &fname) {
    return db::to<db::segment::id>(fname.name);
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
    return position != page::position(0) || segment != db::segment::id(0);
  }
};
} // namespace db
#endif

#ifndef _H_SHARED_IDS_DB
#define _H_SHARED_IDS_DB

#include "conversions.h"
#include "entities.h"
#include <array>
#include <string>

#define DB_ID_CLASS()                                                          \
  struct id {                                                                  \
    using id_type = uint64_t;                                                  \
    id_type _id;                                                               \
    constexpr id() : _id() {                                                   \
    }                                                                          \
    constexpr explicit id(id_type p_id) : _id(p_id) {                          \
    }                                                                          \
                                                                               \
    constexpr id(const id &) = default;                                        \
    constexpr id(id &&) = default;                                             \
    constexpr id &operator=(const id &) = default;                             \
    constexpr id &operator=(id &&) = default;                                  \
                                                                               \
    constexpr id operator+(uint64_t i) const {                                 \
      return id(_id + i);                                                      \
    }                                                                          \
    constexpr bool operator==(const id &o) const {                             \
      return _id == o._id;                                                     \
    }                                                                          \
    constexpr bool operator!=(const id &o) const {                             \
      return !operator==(o);                                                   \
    }                                                                          \
    constexpr bool operator<(const id &o) const {                              \
      return _id < o._id;                                                      \
    }                                                                          \
    constexpr bool operator>(const id &o) const {                              \
      return _id > o._id;                                                      \
    }                                                                          \
  };

namespace journal {
DB_ID_CLASS()
constexpr id NO_ID(0);
constexpr id START_ID(NO_ID + 1);

} // namespace journal

namespace tx {
DB_ID_CLASS()
constexpr id NO_ID(0);
constexpr id START_ID(NO_ID + 1);

} // namespace tx

namespace page {
using position = size_t;

} // namespace page

namespace db {

namespace table {
// transient id used internally to identify tables
DB_ID_CLASS()
constexpr id NO_ID(0);
constexpr id START_ID(NO_ID + 1);
using version = uint64_t;

namespace name {

constexpr size_t length(64);
using type = std::array<char, length>;
} // namespace name
} // namespace table

namespace segment {

// using file_id = uint64_t;

using version = uint16_t;

DB_ID_CLASS()
constexpr id NO_ID(0);
constexpr id START_ID(NO_ID + 1);
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

    ::sprintf(buf, "%lu", id._id);
    return std::string{buf};
  }

  static segment::id id(const db::Filename &fname) {
    using id_type = db::segment::id::id_type;
    return db::segment::id(db::to<id_type>(fname.name));
  }
};

template <typename Table_t>
struct Reservation {
  const db::segment::id segment;
  const page::position position;

  explicit Reservation() : Reservation(db::segment::NO_ID, 0) {
    // TODO fix this
  }

  explicit Reservation(db::segment::id p_id, page::position pos)
      : segment{p_id}, position{pos} {
  }
  bool is_valid() const {
    return position != page::position(0) || segment != db::segment::NO_ID;
  }
};

struct TableSegment {
  const db::table::id table;
  const db::segment::id segment;
  TableSegment(const db::table::id &t, const db::segment::id &s)
      : table(t), segment(s) {
  }
};
} // namespace db
#endif

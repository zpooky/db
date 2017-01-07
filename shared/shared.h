#ifndef _H_SHARED_IDS
#define _H_SHARED_IDS

#include "conversions.h"
#include "entities.h"
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

namespace segment {

using file_id = uint64_t;

using version = uint16_t;

using id = uint64_t;
constexpr id NO_ID(0);
constexpr id START_ID(1);
}
namespace raw {

using size = uint64_t;

using id = uint64_t;
constexpr id START_ID(1);
constexpr id EMPTY(0ul);

using version_t = uint64_t;
constexpr version_t START_VERSION(1);
}

namespace transaction {

using id = uint64_t;
}

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

class Transaction {
private:
  // journal::Journals<hash_t> &journal;
public:
  const db::transaction::id tid;
  const journal::id jid;
  explicit Transaction(journal::id id) : tid(0), jid(id) {
  }
  // Transaction(const Transaction&) = delete;
  ~Transaction() {
    // journal.commit(jid);
  }
};
}
#endif

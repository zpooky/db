#ifndef JOURNAL_LINE_H
#define JOURNAL_LINE_H

#include "../config/Configuration.h"
#include "../shared/entities.h"
#include "../shared/fs.h"

namespace journal {

enum class State : uint8_t { START, COMMIT, INTERNAL };

using name_type = db::table::name::type;
using segment_id = db::segment::id;

template <typename hash_algh>
struct JournalLine {
private:
  using hash_type = typename hash_algh::type;

public:
  const hash_type hash;
  const journal_id id;
  const name_type table;
  const segment_id idx;
  const State state;

public:
  JournalLine(const hash_type &p_hash, journal_id p_id,
              const name_type &p_table, segment_id p_index, State p_state)
      : hash{p_hash}, id{p_id}, table{p_table}, idx{p_index}, state{p_state} {
  }
};

template <typename hash_algh>
JournalLine<hash_algh> empty_segment_line() {
  typename hash_algh::type h{0};
  name_type name{0};
  segment_id i{0};
  State s = State::INTERNAL;
  return {h, 0, name, i, s};
}

template <typename hash_algh>
JournalLine<hash_algh> segment_line(journal_id p_id, const name_type &p_table,
                                    segment_id seg_id, State p_state) {
  hash_algh h;
  h.update(p_id);
  h.update(p_table);
  h.update(seg_id);
  h.update(static_cast<std::underlying_type<State>::type>(p_state));
  return {h.digest(), p_id, p_table, seg_id, p_state};
}
}
#endif

#ifndef JOURNAL_LINE_H
#define JOURNAL_LINE_H

#include "../shared/entities.h"
#include "../shared/shared.h"

namespace journal {

enum class State : uint8_t {
  /* Start the journal transaction
   */
  BEGIN,
  /* specific entry for either
   * - semgent journal entry
   * - line journal entry
   */
  ENTRY,
  /*
   */
  COMMIT,
  /*
   */
  ROLLBACK,
  /*
   */
  INTERNAL
};

template <typename hash_t>
struct JournalLine {
private:
  using hash_type = typename hash_t::type;
  using name_type = db::table::name::type;
  using segment_id = db::segment::id;

public:
  /* A hash of the JournalLine itself
   */
  const hash_type hash;
  /* Id to group related journal entries.
   * journalId<->transactionId
   */
  const journal::id id;
  /* Table name to represent what table
   */
  const name_type table;
  // #<{(| Segment id to represent which segment file
  //  |)}>#
  // const segment_id idx;
  /*
   */
  const State state;

public:
  JournalLine(const hash_type &p_hash, journal::id p_id,
              const name_type &p_table, State p_state)
      : hash{p_hash}, id{p_id}, table{p_table}, state{p_state} {
  }
};

template <typename hash_t>
JournalLine<hash_t> segment_line(journal::id p_id,
                                 const db::table::name::type &p_table,
                                 State p_state) {
  hash_t h;
  h.update(p_id);
  h.update(p_table);
  h.update(static_cast<std::underlying_type<State>::type>(p_state));
  return {h.digest(), p_id, p_table, p_state};
}

template <typename hash_t>
JournalLine<hash_t> empty_segment_line() {
  using name_type = db::table::name::type;
  name_type name{0};
  return segment_line<hash_t>(journal::NO_ID, name, State::INTERNAL);
}

template <typename hash_t>
JournalLine<hash_t> xbegin(journal::id jid,
                           const db::table::name::type &table) {
  return segment_line<hash_t>(jid, table, State::BEGIN);
}

template <typename hash_t>
JournalLine<hash_t> create() {
}
template <typename hash_t>
JournalLine<hash_t> commitx(journal::id jid,
                          const db::table::name::type &table) {
  return segment_line<hash_t>(jid, table, State::COMMIT);
}
}
#endif

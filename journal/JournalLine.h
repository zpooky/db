#ifndef JOURNAL_LINE_H
#define JOURNAL_LINE_H

#include "../shared/Buffer.h"
#include "../shared/entities.h"
#include "../shared/shared.h"

namespace journal {

enum class Type : uint8_t {
  /* Start the journal transaction
   */
  BEGIN = 0,
  /* specific entry for either
   * - semgent journal entry
   * - line journal entry
   */
  ENTRY = 1,
  /*
   */
  COMMIT = 2,
  /*
   */
  ROLLBACK = 3,
  /*
   */
  INTERNAL = 4
};
enum class EntryType : uint8_t {
  /**
   */
  NOP = 0,
  /** represent a segmet create entry
   */
  SEGMENT = 1,
  /** represent a line change
   */
  LINE = 2
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
  /* Table name to represent what table
   */
  const name_type table;
  /* Id to group related journal entries.
   * journalId<->transactionId
   */
  const journal::id id;
  // #<{(| Segment id to represent which segment file
  //  |)}>#
  // const segment_id idx;
  /*
   */
  const Type type;
  /**
   */
  const EntryType entry_type;
  /**
   */
  db::HeapBuffer buffer;

public:
  JournalLine(const hash_type &p_hash, journal::id p_id,
              const name_type &p_table, Type p_type, EntryType p_entry_type,
              db::HeapBuffer &&b)
      : hash{p_hash}, table{p_table}, id{p_id}, type{p_type},
        entry_type{p_entry_type}, buffer{b} {
  }
  JournalLine()
      : hash{0}, table{0}, id{journal::NO_ID}, type{Type::INTERNAL},
        entry_type{EntryType::NOP}, buffer{} {
  }
};

template <typename hash_t>
JournalLine<hash_t>
segment_line(journal::id p_id, const db::table::name::type &p_table,
             Type p_type, EntryType p_entry_type, db::HeapBuffer &&b) {
  hash_t h;
  h.update(p_id);
  h.update(p_table);
  h.update(static_cast<std::underlying_type<Type>::type>(p_type));
  h.update(static_cast<std::underlying_type<EntryType>::type>(p_entry_type));
  h.update(b);
  return {h.digest(), p_id, p_table, p_type, p_entry_type, std::move(b)};
}
namespace line {
template <typename hash_t>
JournalLine<hash_t> segment_line(journal::id p_id,
                                 const db::table::name::type &p_table,
                                 Type p_type) {
  return segment_line<hash_t>(p_id, p_table, p_type, EntryType::NOP,
                              db::HeapBuffer());
}

template <typename hash_t>
JournalLine<hash_t> empty_segment_line() {
  using name_type = db::table::name::type;
  name_type name{0};
  return segment_line<hash_t>(journal::NO_ID, name, Type::INTERNAL);
}

template <typename hash_t>
JournalLine<hash_t> create(journal::id jid, const db::table::name::type &table,
                           EntryType type, db::HeapBuffer &&b) {
  assert(type != EntryType::NOP);
  return segment_line<hash_t>(jid, table, Type::ENTRY, type, std::move(b));
}

template <typename hash_t>
JournalLine<hash_t> begin(journal::id jid, const db::table::name::type &table) {
  return segment_line<hash_t>(jid, table, Type::BEGIN);
}

template <typename hash_t>
JournalLine<hash_t> commit(journal::id jid,
                           const db::table::name::type &table) {
  return segment_line<hash_t>(jid, table, Type::COMMIT);
}
}
}
#endif

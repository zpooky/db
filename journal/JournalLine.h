#ifndef JOURNAL_LINE_H
#define JOURNAL_LINE_H

#include "../shared/Buffer.h"
#include "../shared/entities.h"
#include "../shared/hash.h"
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

struct JournalLine {
public:
  using hash_t = sp::crc32;
  using hash_type = typename hash_t::type;

private:
  using name_type = db::table::name::type;
  using segment_id = db::segment::id;
  // TODO encapsulate
public:
  /* A hash of the JournalLine itself
   */
  hash_type hash;
  /* Table name to represent what table
   */
  name_type table;
  /* Id to group related journal entries.
   * journalId<->transactionId
   */
  journal::id id;
  // #<{(| Segment id to represent which segment file
  //  |)}>#
  // const segment_id idx;
  /*
   */
  Type type;
  /**
   */
  EntryType entry_type;
  /**
   */
  db::HeapBuffer buffer;

public:
  JournalLine(const hash_type &p_hash, journal::id p_id,
              const name_type &p_table, Type p_type, EntryType p_entry_type,
              db::HeapBuffer &&b)
      : hash{p_hash}, table{p_table}, id{p_id}, type{p_type},
        entry_type{p_entry_type}, buffer{std::move(b)} {
  }
  JournalLine()
      : hash{0}, table{0}, id{journal::NO_ID}, type{Type::INTERNAL},
        entry_type{EntryType::NOP}, buffer{} {
  }
  JournalLine(const JournalLine &o)
      : hash{o.hash}, table{o.table}, id{o.id}, type{o.type},
        entry_type{o.entry_type}, buffer{o.buffer} {
  }

  JournalLine &operator=(const JournalLine &o) {
    if (this != &o) {
      return assign(o);
    }
    return *this;
  }

private:
  JournalLine &assign(JournalLine o) {
    swap(o);
    return *this;
  }

public:
  void swap(JournalLine &o) {
    std::swap(hash, o.hash);
    std::swap(table, o.table);
    std::swap(id, o.id);
    std::swap(type, o.type);
    std::swap(entry_type, o.entry_type);
    std::swap(buffer, o.buffer);
  }
};

JournalLine segment_line(const journal::id &jid,
                          const db::table::name::type &t, Type type,
                          EntryType etype, db::HeapBuffer &&b) {
  typename JournalLine::hash_t h;
  h.update(jid);
  h.update(t);
  h.update(static_cast<std::underlying_type<Type>::type>(type));
  h.update(static_cast<std::underlying_type<EntryType>::type>(etype));
  h.update(b);
  return {h.digest(), jid, t, type, etype, std::move(b)};
}

namespace line {
JournalLine segment_line(const journal::id &jid, Type type) {
  return segment_line(jid, {0}, type, EntryType::NOP, db::HeapBuffer(0));
}

// JournalLine empty_segment_line() {
//   using name_type = db::table::name::type;
//   constexpr name_type name{0};
//   return segment_line(journal::NO_ID, name, Type::INTERNAL );
// }

JournalLine create(journal::id jid, const db::table::name::type &table,
                   EntryType type, db::HeapBuffer &&b) {
  assert(type != EntryType::NOP);
  return segment_line(jid, table, Type::ENTRY, type, std::move(b));
}

JournalLine begin(journal::id jid) {
  return segment_line(jid, Type::BEGIN);
}

JournalLine commit(journal::id jid) {
  return segment_line(jid, Type::COMMIT);
}
} // namespace line
} // namespace journal
#endif

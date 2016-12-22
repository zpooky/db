#include <iostream>

#include "../fs/Line.h"
#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/SegmentReservations.h"
#include "../segment/Segments.h"
#include "../shared/TableMeta.h"
#include "../shared/vfs.h"
#include "TestTable.h"

using namespace journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;

namespace db {
struct Transaction {
  const db::transaction::id tid;
  const journal::id jid;
  Transaction(journal::id id) : tid(0), jid(id) {
  }
};
template <typename hash_t>
class Tx {
private:
  Journals<hash_t> &m_jorunal;

public:
  explicit Tx(Context<hash_t> &ctx) : m_jorunal(ctx.journal()) {
  }
  template <typename Table_t>
  Transaction begin() {
    auto id = m_jorunal.begin<Table_t>();
    return {id};
  }
};
}

template <typename hash_t, typename Meta_t>
class Store {
private:
  using Table = typename Meta_t::Table;
  db::Context<hash_t> &m_ctx;
  std::unique_ptr<Segments<Meta_t>> m_segments;
  Journals<hash_t> &m_journals;

public:
  explicit Store(db::Context<hash_t> &ctx)
      : m_ctx(ctx), m_segments(make_unique<Segments<Meta_t>>(m_ctx)),
        m_journals(ctx.journal()) {
  }

  db::transaction::xid_t create(const db::Transaction &t, const Table &data) {
    auto res = m_segments->reserve();
    m_journals.create(t.jid, res, data);
    // m_segments->write(res, data);
    return 0;
  }

  Table read(const db::Transaction &, db::raw::id);
  Table read(const db::Transaction &, db::raw::id, db::raw::version_t);

  db::raw::version_t update(const db::Transaction &, const Table &);
  // db::raw::version_t update(const db::Transaction &,
  // db::transaction::version_t, const Table &);

  void del(const db::Transaction &, db::transaction::xid_t);
  // void del(const db::Transaction &, db::transaction::xid_t,
  // db::raw::version_t);
};

int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_t = crc32;
  using TTT = db::TableMeta<TestTable, hash_t>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);
  db::Context<hash_t> ctx{root};
  Store<hash_t, TTT> store{ctx};
  db::Tx<hash_t> tx{ctx};
  {
    auto t = tx.begin<TestTable>();
    TestTable entry;
    store.create(t, entry);

    // auto jid = journal.begin();
    // auto reserv = segments->reserve();
    // // auto line = to_line<TestTable,hash_t>(move(table));
    // // segment.write(reserv, table);
    // journal.prepare(jid, reserv);
    // journal.commit(jid);
  }

  return 0;
}

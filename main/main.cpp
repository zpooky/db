#include <iostream>

#include "../fs/Line.h"
#include "../journal/Journal.h"
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

using version_t = uint64_t;
using xid_t = uint64_t;

namespace db {
  using transaction_id = size_t;
struct Transaction {
  const transaction_id tid;
  const journal_id jid;
  Transaction(journal_id id) :tid(0), jid(id) {
  }
};
class Tx {
private:
  Journal &m_jorunal;

public:
  explicit Tx(Journal &j) : m_jorunal(j) {
  }
  Transaction begin() {
    auto id = m_jorunal.begin();
    return {id};
  }
};
}

template <typename t_halgh, typename T_Meta>
class Store {
private:
  using Table = typename T_Meta::Table;
  db::Context<t_halgh> &m_ctx;
  unique_ptr<Segments<T_Meta>> m_segments;

public:
  explicit Store(db::Context<t_halgh> &ctx)
      : m_ctx(ctx), m_segments(make_unique<Segments<T_Meta>>(m_ctx)) {
  }

  xid_t create(const db::Transaction &, const Table &) {
    return 0;
  }

  Table read(const db::Transaction &, xid_t);
  Table read(const db::Transaction &, xid_t, version_t);

  version_t update(const db::Transaction &, xid_t, const Table &);
  version_t update(const db::Transaction &, xid_t, version_t, const Table &);

  void del(const db::Transaction &, xid_t);
  void del(const db::Transaction &, xid_t, version_t);
};

int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_algh = crc32;
  using TTT = db::TableMeta<TestTable, hash_algh>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);
  db::Context<hash_algh> ctx{root};
  Store<hash_algh, TTT> store{ctx};
  auto &journal = journal::instance();
  db::Tx tx{journal};
  {
    auto t = tx.begin();
    TestTable entry;
    store.create(t, entry);

    // auto jid = journal.begin();
    // auto reserv = segments->reserve();
    // // auto line = to_line<TestTable,hash_algh>(move(table));
    // // segment.write(reserv, table);
    // journal.prepare(jid, reserv);
    // journal.commit(jid);
  }

  return 0;
}

#include <iostream>

#include "../fs/Line.h"
#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/SegmentReservations.h"
#include "../segment/Segments.h"
#include "../shared/vfs.h"
#include "PageFilesParser.h"
#include "TableMeta.h"
#include "TestTable.h"

using namespace journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;

namespace db {
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
template <typename hash_t>
class Tx {
private:
  Journals<hash_t> &m_jorunal;

public:
  explicit Tx(Context<hash_t> &ctx) : m_jorunal(ctx.journal()) {
  }
  template <typename Meta_t>
  Transaction begin() {
    auto id = m_jorunal.begin<Meta_t>();
    return Transaction{id};
  }
};
}

template <typename Meta_t>
class Store {
private:
  using Table = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using page_t = typename Meta_t::Page;

private:
  db::Context<hash_t> &m_ctx;
  std::unique_ptr<Segments<Meta_t>> m_segments;
  Journals<hash_t> &m_journals;

private:
public:
  explicit Store(db::Context<hash_t> &ctx)
      : m_ctx(ctx), m_segments(nullptr), m_journals(ctx.journal()) {

    auto segment_root = ctx.root().cdx(Meta_t::table_name());
    vfs::mkdir(segment_root);

    page::PageFilesParser<Meta_t> parser(ctx, segment_root);
    m_segments.reset(parser());
  }
  db::raw::id create(const db::Transaction &t, const Table &data) {
    auto res = m_segments->reserve();
    m_journals.template create<Meta_t>(t.jid, res, data);
    return m_segments->create(res, data);
  }

  Table read(const db::Transaction &, db::raw::id);
  /**
   * Optimistic locking
   */
  // Table read(const db::Transaction &, db::raw::id, db::raw::version_t);

  // db::raw::version_t update(const db::Transaction &, const Table &);
  /**
   * Optimistic locking
   */
  // db::raw::version_t update(const db::Transaction &,
  // db::transaction::version_t, const Table &);

  void del(const db::Transaction &);
  /**
   * Optimistic locking
   */
  // void del(const db::Transaction &, db::raw::version_t);
};

int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_t = crc32;
  using TTT = db::TableMeta<TestTable, hash_t>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);
  db::Context<hash_t> ctx{root};
  Store<TTT> store{ctx};
  db::Tx<hash_t> tx{ctx};
  {
    auto t = tx.begin<TTT>();
    TestTable entry;
    store.create(t, entry);
  }

  return 0;
}

#ifndef _DB_STORE_H
#define _DB_STORE_H

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../segment/Segments.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/transaction.h"
#include "PageFilesParser.h"
#include "Tx.h"

namespace db {

template <typename Meta_t>
class Store {
private:
  using Table = typename Meta_t::latest;
  using hash_t = typename Meta_t::hash_t;
  using page_t = typename Meta_t::Page;

private:
  Context<hash_t> &m_ctx;
  tx::Tx<hash_t> &m_tx;
  journal::Journals<hash_t> &m_journals;
  std::unique_ptr<Segments<Meta_t>> m_segments;
  const db::table::id m_id;
  // tx::LineAtomicity m_atomicity;

private:
public:
  explicit Store(db::Context<hash_t> &ctx, tx::Tx<hash_t> &tx)
      : m_ctx(ctx), m_tx(tx), m_journals(ctx.journal()), m_segments(nullptr),
        m_id(ctx.next_id()) //, m_atomicity{}
  {

    auto &config = ctx.config();
    auto &root = config.root;
    auto table_name = Meta_t::table_name();
    auto table_root = root.cdx(table_name);
    vfs::mkdir(table_root);

    page::PageFilesParser<Meta_t> parser(m_id, ctx, table_root, tx);
    m_segments.reset(parser());
    std::atomic_thread_fence(std::memory_order_release);
  }

  Store(const Store<Meta_t> &&) = delete;
  Store(const Store<Meta_t> &) = delete;
  ~Store() {
  }

public:
  db::raw::id create(const tx::Transaction &t, const Table &data) {
    auto res = m_segments->reserve();
    m_journals.template create<Meta_t>(t.jid, res, data);
    return m_segments->create(res, data);
  }

  Table read(const tx::Transaction &, db::raw::id id) {
    // tx::ReadIntention r{id};
    return Table{};
  }
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

  void remove(const tx::Transaction &, db::raw::id id) {
    // tx::RemoveIntention{id};
  }
  /**
   * Optimistic locking
   */
  // void del(const db::Transaction &, db::raw::version_t);
};
}
namespace {
struct TransactionalStore {};
}

#endif

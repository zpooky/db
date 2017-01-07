#ifndef _DB_STORE_H
#define  _DB_STORE_H

#include "../segment/Segments.h"
#include "PageFilesParser.h"
#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../transaction/transaction.h"

namespace db {

template <typename Meta_t>
class Store {
private:
  using Table = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using page_t = typename Meta_t::Page;

private:
  Context<hash_t> &m_ctx;
  std::unique_ptr<Segments<Meta_t>> m_segments;
  journal::Journals<hash_t> &m_journals;

private:
public:
  explicit Store(db::Context<hash_t> &ctx)
      : m_ctx(ctx), m_segments(nullptr), m_journals(ctx.journal()) {

    auto &root = ctx.root();
    auto segment_root = root.cdx(Table::table_name());
    vfs::mkdir(segment_root);

    page::PageFilesParser<Meta_t> parser(ctx, segment_root);
    m_segments.reset(parser());
  }

  Store(const Store<Meta_t> &&) = delete;
  Store(const Store<Meta_t> &) = delete;

public:
  db::raw::id create(const tx::Transaction &t, const Table &data) {
    auto res = m_segments->reserve();
    m_journals.template create<Meta_t>(t.jid, res, data);
    return m_segments->create(res, data);
  }

  Table read(const tx::Transaction &, db::raw::id);
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

  void del(const tx::Transaction &);
  /**
   * Optimistic locking
   */
  // void del(const db::Transaction &, db::raw::version_t);
};

}

#endif

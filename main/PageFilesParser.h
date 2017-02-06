#ifndef _DB_PAGE_FILES_PARSER
#define _DB_PAGE_FILES_PARSER

#include "../fs/Line.h"
#include "../page/FilePage.h"
#include "../page/MaxIdReplay.h"
#include "../page/format/ReplayPageFile.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/Segments.h"
#include "../shared/entities.h"
#include "../transaction/PresentSetReplay.h"
#include "SegmentBuilder.h"
#include "TransactionalGuard.h"
#include "TransactionalSegments.h"
#include "Tx.h"
#include <functional>
#include <vector>

namespace page {
template <typename Meta_t>
class PageFilesParser {
private:
  using Table_t = typename Meta_t::latest;
  using PageFactory = typename Meta_t::PageFactory;
  using Line_t = db::Line<Table_t>;

private:
  db::Context &m_context;
  const db::Directory m_root;
  const db::table::id m_table;
  tx::Tx &m_tx;

public:
  PageFilesParser(db::table::id id, db::Context &ctx, const db::Directory &root,
                  tx::Tx &tx)
      : m_context(ctx), m_root(root.cd("segment")), m_table(id), m_tx(tx) {
    vfs::mkdir(m_root);
  }

  PageFilesParser(const PageFilesParser &) = delete;
  PageFilesParser(const PageFilesParser &&) = delete;

  ~PageFilesParser() {
  }

private:
  auto segment_files() const {
    return db::files(m_root);
  }

  db::segment::id max_id(const std::vector<db::File> &segments) const {
    // TODO use a reduce operation instead
    std::vector<db::segment::id> segment_id(segments.size());
    auto convert = [](const auto &file) {
      auto fname = file.filename();
      return db::Segment_name::id(fname);
    };
    {
      auto begin = segments.begin();
      auto end = segments.end();
      std::transform(begin, end, segment_id.begin(), convert);
    }

    auto max_it = std::max_element(segment_id.begin(), segment_id.end());
    auto max = max_it != segment_id.end() ? *max_it : db::segment::NO_ID;
    return max;
  }

  FilePageMeta meta(const db::File &f) const {
    auto id = db::Segment_name::id(f.filename());
    // TODO
    db::table::version v(1);
    return FilePageMeta(id, f, Line_t::size(), Meta_t::extent_lines(), v);
  }

  auto parse(const std::vector<db::File> &files, MaxIdReplay &idr,
             tx::PresentSetReplay &psr) const {
    // TODO preallocate vector
    std::vector<db::Segment<Meta_t>> result;

    for (const auto &file : files) {
      auto seg_meta = meta(file);
      // TODO create checksum verify *Builder*
      SegmentBuilder<Meta_t> segmentReplay(seg_meta);
      {
        ReplayPageFile<Meta_t> r(seg_meta);
        using Function_t = std::function<void(const db::LineMeta &)>;
        std::vector<Function_t> xs{segmentReplay, idr, psr};
        r.replay(xs);
      }
      auto segment(segmentReplay.build());
      result.push_back(std::move(segment));
    }
    return result;
  }

public:
  tx::TxSegments<Meta_t> operator()() {
    auto files = segment_files();

    auto max = max_id(files);
    db::segment::id next_id(max + 1);

    db::TableSegment segment(m_table, next_id);
    PageFactory factory(segment, m_context.journal(), m_root);

    MaxIdReplay idReplay;
    tx::PresentSetReplay psReplay(m_table);

    auto segments = parse(files, idReplay, psReplay);
    db::raw::id raw_id(idReplay.next());

    using Segments_t = typename db::Segments<Meta_t>;
    auto segs = new Segments_t(raw_id, std::move(factory), std::move(segments));
    return tx::TxSegments<Meta_t>(segs, tx::TxGuard(m_tx, psReplay.build()));
  }
};
} // namespace page

#endif

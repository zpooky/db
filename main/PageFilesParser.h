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
#include "Tx.h"
#include <functional>
#include <vector>

namespace page {
template <typename Meta_t>
class PageFilesParser {
private:
  using Table_t = typename Meta_t::latest;
  using hash_t = typename Meta_t::hash_t;
  using PageFactory = typename Meta_t::PageFactory;
  using Line_t = db::Line<Table_t, hash_t>;

private:
  db::Context<hash_t> &m_context;
  const db::Directory m_root;
  tx::Tx<hash_t> &m_tx;

public:
  PageFilesParser(db::Context<hash_t> &ctx, const db::Directory &root,
                  tx::Tx<hash_t> &tx)
      : m_context(ctx), m_root(root.cd("segment")), m_tx(tx) {
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

  auto parse(const std::vector<db::File> &files, MaxIdReplay &idReplay,
             tx::PresentSetReplay &psReplay) const {
    // TODO preallocate vector
    std::vector<db::Segment<Meta_t>> result;

    for (const auto &file : files) {
      auto seg_meta = meta(file);
      // TODO create checksum verify *Builder*
      SegmentBuilder<Meta_t> segmentReplay(seg_meta);
      {
        ReplayPageFile<Meta_t> r(seg_meta);
        using Function_t = std::function<void(const db::LineMeta &)>;
        std::vector<Function_t> xs{segmentReplay, idReplay, psReplay};
        r.replay(xs);
      }
      auto segment(segmentReplay.build());
      result.push_back(std::move(segment));
    }
    return result;
  }

public:
  db::Segments<Meta_t> *operator()() {
    auto files = segment_files();

    auto max = max_id(files);
    db::segment::id next_id(max + 1);

    PageFactory factory(m_context, next_id, m_root);

    MaxIdReplay idReplay;
    tx::PresentSetReplay psReplay;
    auto segments = parse(files, idReplay, psReplay);
    auto raw_id = idReplay.next();

    using Segments_t = typename db::Segments<Meta_t>;
    return new Segments_t(raw_id, std::move(factory), std::move(segments));
  }
};
}

#endif

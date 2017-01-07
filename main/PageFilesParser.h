#ifndef _DB_PAGE_FILES_PARSER
#define _DB_PAGE_FILES_PARSER

#include "../page/FilePage.h"
#include "../page/format/ReplayPageFile.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/Segments.h"
#include "../shared/entities.h"
#include "SegmentBuilder.h"
#include <vector>

namespace page {
template <typename Meta_t>
class PageFilesParser {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using PageFactory = typename Meta_t::PageFactory;
  using Line_t = db::Line<Table_t, hash_t>;

private:
  db::Context<hash_t> &m_context;
  const db::Directory m_root;

public:
  explicit PageFilesParser(db::Context<hash_t> &ctx, const db::Directory &root)
      : m_context(ctx), m_root(root.cd("segment")) {
    vfs::mkdir(m_root);
  }

  PageFilesParser(const PageFilesParser &) = delete;
  PageFilesParser(const PageFilesParser &&) = delete;

private:
  auto segment_files() const {
    return db::files(m_root);
  }

  db::segment::id max_id(const std::vector<db::File> &segments) const {
    // TODO use a reduce operation instead
    std::vector<db::segment::id> num_segments(segments.size());
    auto convert = [](const auto &file) {
      auto fname = file.filename();
      return db::Segment_name::id(fname);
    };
    std::transform(segments.begin(), segments.end(), num_segments.begin(),
                   convert);

    auto max_it = std::max_element(num_segments.begin(), num_segments.end());
    auto max = max_it != num_segments.end() ? *max_it : db::segment::START_ID;
    return max;
  }

  FilePageMeta meta(const db::File &f) const {
    //TODO
    auto id = db::Segment_name::id(f.filename());
    db::table::version v(1);
    return FilePageMeta(id, f, Line_t::size(), Meta_t::extent_lines(), v);
  }

  auto init(const std::vector<db::File> &files) const {
    std::vector<db::Segment<Meta_t>> result;
    // TODO preallocate vector
    for (const auto &file : files) {
      auto seg_meta = meta(file);
      ReplayPageFile<Meta_t> r(seg_meta);
      // TODO create checksum verify *Builder*
      SegmentBuilder<Meta_t> segment(seg_meta);
      std::vector<SegmentBuilder<Meta_t>> xs{segment};
      r.replay(xs);

      result.push_back(segment.build());
    }
    return result;
  }

public:
  db::Segments<Meta_t> *operator()() {
    auto segments = segment_files();
    auto max = max_id(segments);

    db::segment::id next_id(max + 1);
    PageFactory factory(m_context, next_id, m_root);

    using Segment_t = typename db::Segments<Meta_t>;
    return new Segment_t(next_id, std::move(factory), init(segments));
  }
};
}

#endif

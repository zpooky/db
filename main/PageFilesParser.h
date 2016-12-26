#ifndef _DB_PAGE_FILES_PARSER
#define _DB_PAGE_FILES_PARSER

#include "../page/FilePage.h"
#include "../page/format/DD.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/Segments.h"
#include "../shared/entities.h"
#include "PageFileParser.h"
#include <vector>

namespace page {
template <typename Meta_t>
class PageFilesParser {
private:
  using hash_t = typename Meta_t::hash_algh;
  using PageFactory = typename Meta_t::PageFactory;

private:
  db::Context<hash_t> &m_context;
  const db::Directory m_root;
  DD parser;

public:
  explicit PageFilesParser(db::Context<hash_t> &ctx, const db::Directory &root)
      : m_context(ctx), m_root(root.cd("segment")), parser(m_root) {
    vfs::mkdir(m_root);
  }

  PageFilesParser(const PageFilesParser &) = delete;
  PageFilesParser(const PageFilesParser &&) = delete;

private:
  auto init(const std::vector<db::File> &files) const {
    std::vector<db::fs::Segment<Meta_t>> result;
    // TODO preallocate vector
    for (const auto &file : files) {
      PageFileParser<Meta_t> parser(file);
      result.push_back(parser.parse());
    }
    return result;
  }

public:
  db::fs::Segments<Meta_t> *operator()() {
    db::segment::id seg_cnt;
    std::vector<db::File> segments;
    std::tie(seg_cnt, segments) = parser();

    db::segment::id next_id = db::segment::id(seg_cnt + 1);
    PageFactory factory(m_context, next_id, m_root);

    return new db::fs::Segments<Meta_t>(std::move(factory), init(segments));
  }
};
}

#endif

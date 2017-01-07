#ifndef _DB_INDEX_INDEX_BUILDER
#define _DB_INDEX_INDEX_BUILDER

#include "../fs/Line.h"
#include "UniqueIndex.h"

namespace index {
template <typename Meta_t>
class IndexBuilder {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using Line_t = db::Line<Table_t, hash_t>;

public:
  // void operator()(const Line_t &) {
  // }

  UniqueIndex build(){
    return UniqueIndex{};
  }
};
}
#endif

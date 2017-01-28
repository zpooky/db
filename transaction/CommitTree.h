#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H
#include <stdint.h>
#include "../shared/shared.h"

namespace tx {

class CommitTree {
private:
  const db::table::id m_id;

public:
  explicit CommitTree(db::table::id id) : m_id(id) {
  }
};
}
#endif

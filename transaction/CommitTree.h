#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H

#include "../shared/ExtentSet.h"
#include "../shared/shared.h"
#include <stdint.h>

namespace tx {

class CommitTree {
private:
  const db::table::id m_table;
  db::ExtentSet<32> es;

public:
  explicit CommitTree(const db::table::id &table)
      : m_table(table), es(db::segment::id(0)) {
  }

  // void introduce(const db::ExtentSet &) {
  // }
};
}
#endif

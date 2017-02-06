#include <iostream>

#include "../segment/Context.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"
#include "Store.h"
#include "TestTable.h"
#include "Tx.h"

using namespace journal;
using namespace db::fs;
using namespace std;

int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;

  db::Directory root("/tmp/db");
  db::Configuration conf(root);
  vfs::mkdir(root);

  db::Context ctx(conf);

  tx::Tx tx{ctx};
  db::Store<TestTableMetax> t1_store{ctx, tx};
  db::Store<Test2TableMetax> t2_store{ctx, tx};
  {
    for (size_t i(0); i < TestTableMetax::extent_lines() * 4; ++i) {
      auto t = tx.begin();
      TestTable entry;
      auto tid = t1_store.create(t, entry);
      t1_store.read(t, tid);
      t1_store.remove(t, tid);
      tx.commit(t);
    }
    auto t = tx.begin();
    Test2Table entry2;
    t2_store.create(t, entry2);
    tx.commit(t);
  }

  return 0;
}

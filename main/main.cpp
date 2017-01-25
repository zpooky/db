#include <iostream>

#include "../segment/Context.h"
#include "../shared/hash.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"
#include "Store.h"
#include "TableMeta.h"
#include "TestTable.h"
#include "Tx.h"

using namespace journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;


int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_t = crc32;
  using Test1Meta = db::TableMeta<TestTable, hash_t>;
  using Test2Meta = db::TableMeta<Test2Table, hash_t>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);

  db::Context<hash_t> ctx(root);

  db::Store<Test1Meta> t1_store{ctx};
  db::Store<Test2Meta> t2_store{ctx};
  tx::Tx<hash_t> tx{ctx};
  {
    auto t = tx.begin();
    for (size_t i(0); i < Test1Meta::extent_lines() * 4; ++i) {
      TestTable entry;
      auto tid = t1_store.create(t, entry);
      t1_store.read(t, tid);
      t1_store.remove(t, tid);
    }
    Test2Table entry2;
    t2_store.create(t, entry2);
    tx.commit(t);
  }

  return 0;
}

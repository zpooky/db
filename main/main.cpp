#include <iostream>

#include "../segment/Context.h"
#include "../shared/hash.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"
#include "Store.h"
#include "TableMeta.h"
#include "TestTable.h"

using namespace journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;

namespace db {
template <typename hash_t>
class Tx {
private:
  Journals<hash_t> &m_jorunal;

public:
  explicit Tx(Context<hash_t> &ctx) : m_jorunal(ctx.journal()) {
  }
  tx::Transaction begin() {
    return begin(tx::Settings());
  }
  tx::Transaction begin(tx::Settings&& s) {
    auto id = m_jorunal.begin();
    return tx::Transaction{id, std::move(s)};
  }
};
}

int main(int, char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_t = crc32;
  using Test1Meta = db::TableMeta<TestTable, hash_t>;
  using Test2Meta = db::TableMeta<Test2Table, hash_t>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);

  db::Context<hash_t> ctx{root};
  db::Tx<hash_t> tx{ctx};

  db::Store<Test1Meta> t1_store{ctx};
  db::Store<Test2Meta> t2_store{ctx};
  {
    auto t = tx.begin();
    for (size_t i(0); i < 100000; ++i) {
      TestTable entry;
      t1_store.create(t, entry);
    }
    Test2Table entry2;
    t2_store.create(t, entry2);
  }

  return 0;
}

#include <iostream>

#include "../fs/Line.h"
#include "../journal/Journal.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../segment/SegmentReservations.h"
#include "../segment/Segments.h"
#include "../shared/TableMeta.h"
#include "../shared/vfs.h"
#include "TestTable.h"

using namespace journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;

int main(int , char *[]) {
  cout << "sector size:" << vfs::sector::logical::size("") << endl;
  cout << "page size:" << vfs::page::size() << endl;
  using hash_algh = crc32;
  using TTT = db::TableMeta<TestTable, hash_algh>;

  db::Directory root("/tmp/db");
  vfs::mkdir(root);
  db::Context<hash_algh> ctx{root};
  auto segments = make_unique<Segments<TTT>>(ctx);
  auto &journal = journal::instance();

  {
    TestTable table;

    auto jid = journal.begin();
    auto reserv = segments->reserve();
    // auto line = to_line<TestTable,hash_algh>(move(table));
    // segment.write(reserv, table);
    journal.prepare(jid, reserv);
    journal.commit(jid);
  }

  //    db::TableFacade<TestTable> tf(db::Directory("/tmp"));
  //    auto &reservations = tf.get_reservations();
  //    auto &segment = tf.get_segments();
  //
  //
  //    TestTable table;
  //    auto &journal = db::journal::instance<TestTable>();
  ////    SegmentFile sf(db::File("/tmp/test.db"), 1024l, 1024l);
  ////    Segment<TestTable> segment{sf};
  ////    SegmentReservations<TestTable> reservations{segment};
  //    Entry<Type::ABORT> entry;
  //    cout << static_cast<int>(entry.get_type()) << endl;
  //    {
  //        auto jid = journal.begin();
  //        auto reserv = reservations.reserve();
  //        auto line = to_line(move(table));
  ////        segment.write(reserv, line);
  //        journal.prepare(jid, reserv);
  //        journal.commit(jid);
  //    }
  return 0;
}

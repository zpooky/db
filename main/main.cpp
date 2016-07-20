#include <iostream>

#include "../journal/Journal.h"
#include "../segment/Segment.h"
#include "../segment/SegmentReservations.h"
#include "TestTable.h"
#include "../shared/vfs.h"
#include "../segment/Segments.h"
#include "../shared/TableMeta.h"
#include "../segment/Context.h"


using namespace db::journal;
using namespace db::fs;
using namespace std;
using namespace sp::hash;

int main(int argc, char *args[]) {
    cout << "sector size:" << db::vfs::sector::size("") << endl;
    cout << "page size:" << db::vfs::page::size() << endl;
    using hash_algh = crc32;
    using TTT = db::TableMeta<TestTable, hash_algh>;

    db::Context<hash_algh> ctx{db::Directory("/tmp")};
    Segments<TTT> segments{ctx};
    auto &journal = db::journal::instance<TestTable>();

    {
        TestTable table;

        auto jid = journal.begin();
        auto reserv = segments.reserve();
//        auto line = to_line(move(table));
//        segment.write(reserv, line);
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
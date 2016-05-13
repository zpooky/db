#include <iostream>

#include "../journal/Journal.h"
#include "../fs/segment/Segment.h"
#include "../fs/segment/SegmentReservations.h"
#include "TestTable.h"
#include "../shared/vfs.h"
#include "TableFacade.h"


using namespace db::journal;
using namespace db::fs;
using namespace std;

int main(int argc, char *args[]) {
    cout << "sector size:" << db::vfs::sector::size("") << endl;
    cout << "page size:" << db::vfs::page::size() << endl;

    Context ctx{db::Directory("/tmp"), 0l};
    Segments<TestTable> segments{ctx};
    auto res1 = segments.reserve();





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
#include <iostream>

#include "../journal/Journal.h"
#include "../fs/segment/Reservation.h"
#include "../fs/File.h"
#include "TestTable.h"
#include "../shared/vfs.h"


using namespace db::journal;
using namespace db::fs;
using namespace std;

int main(int argc, char *args[]) {
    VirtualFileSystem vfs;
    cout << "sector size:" << vfs.sector_size("") << endl;

    TestTable table;
    auto journal = Journal::instance(table);
    SegmentFile sf("/tmp/test.db", 1024l);
    Segment<TestTable> segment{sf};
    Reservations<TestTable> reservations{segment};
    Entry<Type::ABORT> entry;
    cout << static_cast<int>(entry.get_type()) << endl;
    {
        auto jid = journal.begin();
        auto reserv = reservations.reserve();
        auto line = to_line(move(table));
        segment.write(reserv, line);
        journal.done(jid, reserv);
        journal.commit(jid);
    }
    return 0;
}
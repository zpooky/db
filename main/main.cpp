#include <iostream>

#include "../journal/Journal.h"
#include "../fs/Reservation.h"
#include "../fs/File.h"
#include "TestTable.h"

using namespace db::journal;
using namespace db::fs;
using namespace std;

int main(int argc, char *args[]) {
    TestTable table;
    auto journal = Journal::instance(table);
    Reservations reservations;
    Entry<Type::ABORT> entry;
    cout << static_cast<int>(entry.get_type()) << endl;
    File f{1};
    {
        auto jid = journal.begin();
        auto reserv = reservations.reserve();
        auto line = db::fs::line(move(table));
        f.write(reserv, line);
        journal.done(jid, reserv);
        journal.commit(jid);
    }
    return 0;
}
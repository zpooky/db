//
// Created by spooky on 2016-03-03.
//

#include "Journal.h"
#include "../fs/segment/Reservation.h"

namespace db {
    namespace journal {
        journal_id Journal::begin() {
            return 1l;
        }

        void Journal::done(journal_id id, const db::Reservation &reservation) {

        }

        void Journal::commit(journal_id id) {

        }
    }
}
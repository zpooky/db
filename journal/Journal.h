//
// Created by spooky on 2016-03-03.
//

#ifndef DB_JOURNAL_H
#define DB_JOURNAL_H

#include "../shared/fs.h"

namespace db {
    namespace journal {
        enum class Type : unsigned char {
            START = 0, UPDATE = 1, COMMIT = 2, ABORT = 3, CHECKPOINT = 4
        };

        template<db::journal::Type TYPE>
        struct Entry {

            Type get_type() const {
                return TYPE;
            }
        };

        class Journal {
        private:
            const db::Table& m_table;

            Journal(const Table &t) : m_table(t) {

            }

        public:
            static Journal instance(const Table &t) {
                return {t};
            }

            journal_id begin();

            void done(journal_id, const db::Reservation &);

            void commit(journal_id);
        };
    }
}


#endif //DB_JOURNAL_H

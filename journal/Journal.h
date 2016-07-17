//
// Created by spooky on 2016-03-03.
//

#ifndef DB_JOURNAL_H
#define DB_JOURNAL_H

#include "../shared/fs.h"
#include "../shared/Assertions.h"

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

        template<typename t_Table>
        class Journal {
        private:
            Journal(const Journal<t_Table> &o) = delete;

        public:

            Journal() {
                assert_is_table<t_Table>();
            }

            journal_id begin();

            void prepare(journal_id, const db::Reservation &);

            void commit(journal_id);
        };

        template<typename t_Table>
        Journal<t_Table> &instance();

        template<typename t_Table>
        Journal<t_Table> &instance() {
            db::assert_is_table<t_Table>();
            static Journal<t_Table> inst{};
            return inst;
        }


        template<typename t_Table>
        journal_id Journal<t_Table>::begin() {
            return 1l;
        }


        template<typename t_Table>
        void Journal<t_Table>::prepare(journal_id id, const db::Reservation &r) {

        }


        template<typename t_Table>
        void Journal<t_Table>::commit(journal_id id) {

        }
    }
}


#endif //DB_JOURNAL_H

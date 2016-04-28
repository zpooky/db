//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../../shared/entities.h"
#include "../../shared/fs.h"
#include "../../shared/hash.h"
#include <atomic>

namespace db {
    namespace fs {
        using std::atomic;

        class SegmentJournalThread {
        private:
        public:


            void operator ()(){

            }
        };


        template<typename t_Table, typename hash_algh = hash::crc32>
        class SegmentJournal {
        private:
            const File &m_journal;
            atomic<journal_id> m_id;
        public:
            SegmentJournal(const File &journal, journal_id start = 0) : m_journal{journal}, m_id{start} {
            }

            journal_id start(const File &file) {
                hash_algh h;
                return ++m_id;
            }

            void prepare(journal_id id){

            }

            void commit(journal_id id) {

            }

        };

    }
}

#endif //PROJECT_SEGMENTJOURNAL_H

//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../../shared/entities.h"
#include "../../shared/fs.h"
#include "../../shared/hash.h"
#include <atomic>
#include "../../collection/Queue.h"
#include "../../config/Configuration.h"
#include <utility>

namespace db {
    namespace fs {
        using std::atomic;

        namespace internal {

            enum class State : unsigned short {
                START, PREPARED, COMMIT
            };

            using name_type = db::table::name::type;
            using index_type = db::segment::index_type;

            template<typename hash_algh>
            struct SegmentLine {
                const typename hash_algh::type hash;
                const journal_id id;
                const name_type table;
                const index_type idx;
                const State state;
            private:
                SegmentLine(typename hash_algh::type p_hash, journal_id p_id, const name_type &p_table,
                            index_type p_index, State p_state)
                        : hash{p_hash}, id{p_id}, table{p_table}, idx{p_index}, state{p_state} {
                }

            public:
                template<typename t_hash_algh>
                static auto make(journal_id p_id, const name_type &p_table, index_type p_index, State p_state) {
                    t_hash_algh h;
                    h.update(p_id);
                    h.update(p_table);
                    h.update(p_index);
                    h.update(p_state);
                    return SegmentLine {h.digest(), p_id, p_table, p_index, p_state};
                }
            };

            template<typename T>
            struct Consumer {
                virtual void add(T &&) = 0;
            };

            template<typename hash_algh>
            class SegmentJournalThread : public Consumer<SegmentLine<hash_algh>> {
            private:
                using SegLine = SegmentLine<hash_algh>;
                sp::Queue<SegLine> m_queue;
            public:

                void add(SegLine &&data) {
                    m_queue.add(std::move(data));
                }

                void operator()() {
                    while (true) {
                        auto entry = m_queue.get();
                        if (entry.id != 0l) {
                        }
                    }
                }
            };
        }
        using internal::SegmentJournalThread;
        using internal::SegmentLine;
        template<typename hash_algh>
        using SLConsumer = internal::Consumer<SegmentLine<hash_algh>>;

        template<typename t_Table, typename hash_algh = hash::crc32>
        class SegmentJournal {
        private:
            using SJState = internal::State;
            const File &m_journal;
            SLConsumer<hash_algh> &m_consumer;
            atomic<journal_id> m_id;
        public:
            SegmentJournal(const File &journal, SLConsumer<hash_algh> &consumer, journal_id start = 0) :
                    m_journal{journal}, m_consumer{consumer}, m_id{start} {
            }

        private:
            using name_type = db::table::name::type;
            using index_type = db::segment::index_type;

            SegmentLine<hash_algh> line(journal_id id, SJState state) const {
                name_type name{0};
                index_type idx = 0;
                return SegmentLine<hash_algh>::make(id, name, idx, state);
            }

            SegmentLine<hash_algh> line(journal_id id, const name_type &name, index_type idx) const {
                return SegmentLine<hash_algh>::make(id, name, idx, SJState::START);
            }

        public:
            journal_id start(const name_type &name, index_type idx) {
                auto id = ++m_id;
                m_consumer.add(line(id, name, idx));
                return id;
            }

            void prepare(journal_id id) {
                m_consumer.add(line(id, SJState::PREPARED));
            }

            void commit(journal_id id) {
                m_consumer.add(line(id, SJState::COMMIT));
            }

        };

    }
}

#endif //PROJECT_SEGMENTJOURNAL_H

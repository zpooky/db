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
#include <thread>

namespace db {
    namespace fs {
        using std::atomic;

        namespace internal {

            enum class State : uint16_t {
                START, PREPARED, COMMIT, INTERNAL
            };

            using name_type = db::table::name::type;
            using segment_id = db::segment::id;

            template<typename hash_algh>
            struct SegmentLine {
                const typename hash_algh::type hash;
                const journal_id id;
                const name_type table;
                const segment_id idx;
                const State state;
            public:
                SegmentLine(typename hash_algh::type p_hash, journal_id p_id, const name_type &p_table,
                            segment_id p_index, State p_state)
                        : hash{p_hash},
                          id{p_id},
                          table{p_table},
                          idx{p_index},
                          state{p_state} {
                }

            };

            template<typename hash_algh>
            SegmentLine<hash_algh> empty_segment_line() {
                typename hash_algh::type h{0};
                name_type name{0};
                segment_id i{0};
                State s = State::INTERNAL;
                return {h, 0, name, i, s};
            }

            template<typename T>
            struct Consumer {
                virtual void add(T &&) = 0;
            };

            template<typename hash_algh>
            class SegmentJournalThread : public Consumer<SegmentLine<hash_algh>> {
            private:
                using SegLine = SegmentLine<hash_algh>;
                sp::Queue<SegLine> m_queue;
                std::atomic<bool> m_interrupted;
            public:
                SegmentJournalThread() :
                        m_queue{db::fs::internal::empty_segment_line<hash_algh>()},
                        m_interrupted{false} {

                }

                SegmentJournalThread(SegmentJournalThread<hash_algh> &&o) :
                        m_queue(std::move(o.m_queue)),
                        m_interrupted{o.m_interrupted} {

                }

                void add(SegLine &&data) {
                    m_queue.push_front(std::move(data));
                }

                void interrupt() {
                    m_interrupted.store(true);
                }

                void operator()() {
                    while (!m_interrupted) {
                        auto entry = m_queue.pop();
                        if (entry.id != 0l) {
                        } else {
                            printf("www");
                        }
                    }
                }
            };
        }

        template<typename hash_algh>
        internal::SegmentLine<hash_algh> segment_line(journal_id p_id, const db::table::name::type &p_table,
                                                      db::segment::id segment_id, internal::State p_state) {
            hash_algh h;
            h.update(p_id);
            h.update(p_table);
            h.update(segment_id);
            h.update(static_cast<uint16_t>(p_state));
            return {h.digest(), p_id, p_table, segment_id, p_state};
        }


        using internal::SegmentJournalThread;
        using internal::SegmentLine;
        template<typename hash_algh>
        using SLConsumer = internal::Consumer<SegmentLine<hash_algh>>;

        template<typename T_Meta>
        class SegmentJournal {
        private:
            using hash_algh = typename T_Meta::hash_algh;
            using T_Table = typename T_Meta::Table;
            using State = internal::State;
            const File m_journal;
            SLConsumer<hash_algh> &m_consumer;
            atomic<journal_id> m_counter;
        public:
            explicit SegmentJournal(const File &journal, SLConsumer<hash_algh> &consumer, journal_id start = 0) :
                    m_journal(journal),
                    m_consumer(consumer),
                    m_counter{start} {
//                db::assert_is_meta<T_Meta>();
            }

        private:
            using name_type = db::table::name::type;
            using segment_id = db::segment::id;

            SegmentLine<hash_algh> line(journal_id id, State state) const {
                name_type name{0};
                segment_id idx = 0;
                return segment_line<hash_algh>(id, name, idx, state);
            }

            SegmentLine<hash_algh> line(journal_id id, const name_type &name, segment_id idx) const {
                return segment_line<hash_algh>(id, name, idx, db::fs::internal::State::START);
            }

        public:

            journal_id start(const name_type &name, segment_id idx) {
                auto id = ++m_counter;
                m_consumer.add(line(id, name, idx));
                return id;
            }

//            void prepare(journal_id id) {
//                m_consumer.add(line(id, State::PREPARED));
//            }

            void commit(journal_id id) {
                m_consumer.add(line(id, State::COMMIT));
            }

        };

    }
}

#endif //PROJECT_SEGMENTJOURNAL_H

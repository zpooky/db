//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../shared/entities.h"
#include "../shared/fs.h"
#include "../shared/hash.h"
#include <atomic>
#include "../collection/Queue.h"
#include "../config/Configuration.h"
#include <utility>
#include <thread>
#include "../fs/FileWriter.h"
#include "Segment.h"

namespace db {
    using std::atomic;

    namespace {

        enum class State : uint8_t {
            START, COMMIT, INTERNAL
        };

        using name_type = db::table::name::type;
        using segment_id = db::segment::id;

        template<typename hash_algh>
        struct SegmentLine {
        private:
            using hash_type = typename hash_algh::type;
        public:
            const hash_type hash;
            const journal_id id;
            const name_type table;
            const segment_id idx;
            const State state;
        public:
            SegmentLine(const hash_type &p_hash, journal_id p_id, const name_type &p_table,
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
            const File m_file;
            sp::Queue<SegLine> m_queue;
            std::atomic<bool> m_interrupted;
        public:
            explicit SegmentJournalThread(const File &seg_file) :
                    m_file(seg_file),
                    m_queue(/*empty_segment_line<hash_algh>()*/),
                    m_interrupted(false) {

            }

            SegmentJournalThread(SegmentJournalThread<hash_algh> &&o) :
                    m_file(o.m_file),
                    m_queue(std::move(o.m_queue)),
                    m_interrupted(o.m_interrupted.load()) {
            }

            SegmentJournalThread(const SegmentJournalThread &) = delete;

            void add(SegLine &&data) {
                m_queue.enqueue(std::move(data));
            }

            void interrupt() {
                m_interrupted.store(true);
            }

            void operator()() {
                db::FileWriter fw(m_file);
                while (!m_interrupted) {
                    auto entries = m_queue.drain();
                }
            }
        };
    }

    template<typename hash_algh>
    SegmentLine<hash_algh> segment_line(journal_id p_id, const name_type &p_table,
                                        segment_id seg_id, State p_state) {
        hash_algh h;
        h.update(p_id);
        h.update(p_table);
        h.update(seg_id);
        h.update(static_cast<std::underlying_type<State>::type>(p_state));
        return {h.digest(), p_id, p_table, seg_id, p_state};
    }


    template<typename hash_algh>
    using Consumer_t = Consumer<SegmentLine<hash_algh>>;

    template<typename hash_algh>
    class SegmentJournal {
    private:
        Consumer_t<hash_algh> &m_consumer;
        atomic<journal_id> m_counter;
    public:
        explicit SegmentJournal(Consumer_t<hash_algh> &consumer, journal_id start = 0) :
                m_consumer(consumer),
                m_counter{start} {
//                db::assert_is_meta<T_Meta>();
        }

        SegmentJournal(SegmentJournal &&o) :
                m_consumer(o.m_consumer),
                m_counter(o.m_counter.load()) {

        }

        SegmentJournal(const SegmentJournal &) = delete;

    private:

        SegmentLine<hash_algh> line(journal_id id, State state) const {
            name_type name{0};
            segment_id idx = 0;
            return segment_line<hash_algh>(id, name, idx, state);
        }

        SegmentLine<hash_algh> line(journal_id id, const name_type &name, segment_id idx) const {
            return segment_line<hash_algh>(id, name, idx, State::START);
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

#endif //PROJECT_SEGMENTJOURNAH

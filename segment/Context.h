//
// Created by spooky on 2016-07-20.
//

#ifndef PROJECT_CONTEXT_H
#define PROJECT_CONTEXT_H

#include "../shared/entities.h"
#include "../config/Configuration.h"

namespace db {
    template<typename hash_algh>
    struct Context {
        using segment_id = db::segment::id;
        const db::Directory root;

        SegmentJournalThread <hash_algh> m_runnable;
        SegmentJournal <hash_algh> m_journal;
        std::thread m_thread;

        explicit Context(const Directory &p_root) :
                root(p_root),
                m_runnable(p_root.cd(Filename("segment.journal"))),
                m_journal(m_runnable)
//                ,m_thread(std::move(m_runnable))
        { }

        Context(const Context &&) = delete;

        Context(const Context &) = delete;

        ~Context() {
            m_runnable.interrupt();
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

    };
}
#endif //PROJECT_CONTEXT_H

//
// Created by spooky on 2016-09-21.
//

#ifndef PROJECT_MPSCFIFO_H
#define PROJECT_MPSCFIFO_H

#include <atomic>
#include <stdexcept>
#include <type_traits>
#include <algorithm>

namespace sp {

    template<typename T>
    class MpScFifo {
    private:
        struct Entry;
        using Entry_t = std::atomic<Entry *>;

        struct Entry {
            virtual std::string to_string() = 0;

            virtual Entry_t &previous() = 0;

            virtual std::string name() = 0;

            virtual ~Entry() {
            }
        };

        struct NoEntry : public Entry {
        private:
            Entry_t prev;
        public:
            NoEntry() : prev{nullptr} {

            }

            std::string to_string() {
                auto node = prev.load();
                if (node) {
                    return node->to_string();
                }
                std::string s;
                return s;
            }

            std::string name() {
                return "NoEntry";
            }

            Entry_t &previous() {
                return prev;
            }
        };

        struct IEntry : public Entry {
        private:
        public:
            T data;
            Entry_t prev;

            explicit IEntry(T &&p_data) :
                    data{std::forward<T>(p_data)},
                    prev{nullptr} {

            }

            Entry_t &previous() {
                return prev;
            }

            std::string name() {
                return "IEntry";
            }

            std::string to_string() {
                std::string s;
                char buf[64];
                sprintf(buf, "%u-", data);
                s.append(buf);

                auto next = prev.load();
                if (next) {
                    s.append(next->to_string());
                }
                return s;
            }
        };

/**
 * m_push <- entryN... <- m_poll
 */
        Entry_t m_push;
        Entry_t m_poll;
        //std::atomic<Entry *> m_poll;
    public:
        MpScFifo() :
                m_push{nullptr},
                m_poll{nullptr} {
//            m_reserved->prev.store(m_reserved);
        }

        MpScFifo(const MpScFifo &) = delete;

        MpScFifo &operator=(const MpScFifo &) = delete;

        MpScFifo(const MpScFifo &&) = delete;

        MpScFifo &operator=(const MpScFifo &&) = delete;

        ~MpScFifo() {
            start:
            auto current = m_poll.load();
            if (!m_poll.compare_exchange_strong(current, nullptr)) {
                goto start;
            }
            while (current) {
                auto previous = current->previous().load();
                delete current;
                current = previous;
            }
        }

        /**
         * push a entry onto the FIFO
         * can be executed by multiple concurrent threads
         */
        template<typename D>
        void push_back(D &&data) {
            static_assert(std::is_same<typename std::remove_reference<D>::type,
                    typename std::remove_reference<T>::type>::value, "Is required to be of same type");
            auto current = new IEntry(std::forward<T>(data));
            begin:
            auto head = m_push.load();
            if (head) {
                Entry *nil = nullptr;
                /* Expect previous to be null otherwise contention and reload head
                 */
                if (!head->previous().compare_exchange_strong(nil, current)) {
                    /* nil is something else. If its NoEntry we can help out by...
                     *
                     */
                    if (NoEntry *no = dynamic_cast<NoEntry *>(nil)) {
                        m_push.compare_exchange_strong(head, no);
//                        auto no_prev = no->previous().load();
//                        if (no_prev) {
////                            if (head->previous().compare_exchange_strong(head, nu)) {
////                                auto kk = 0;
////                            }
//                            if (m_push.compare_exchange_strong(head, no)) {
//                                auto kk = 0;
//                            }
//                            //delete no;
//                        } else {
////sp
//                            bool res = head->previous().compare_exchange_strong(no_prev, current);
//                            if (!res) {
//                                goto begin;
//                            }
//                            return;
//                        }
                    }
                    goto begin;
                }
                /* swap head with current will not race with the 'head->previous' cas since
                 * we will only get to this point if 'head->previous' is null and the previous
                 * cas ensures that only one thread is allowed to change the value from null.
                 *
                 * this cas will publish the local 'current' node as the head of the push chain.
                 */
                bool res = m_push.compare_exchange_strong(head, current);
                if (!res) {
                    std::cout << "cas1\n";
                    exit(1);
//                    throw std::runtime_error{"res false"};
                }
            } else {
                Entry *nil = nullptr;
                if (!m_push.compare_exchange_strong(nil, current)) {
                    goto begin;
                }
                if (!m_poll.compare_exchange_strong(nil, current)) {
                    std::cout << "cas3\n";
                    exit(3);
                }
            }
        }

    public:
        /**
         * this is only allowed to be executed be a single thread
         */
        template<typename D>
        T pop(D &&def) {
            begin:
            //TODO: a failed cas will reread current
            auto current = m_poll.load();
            // check if there is something in the queue
            if (current) {
                auto prev = current->previous().load();
                /** check if current has no parent
                 *  if so current is the head for m_push.
                 */
                if (NoEntry *no = dynamic_cast<NoEntry *>(current)) {
                    if (prev == nullptr) {
                        goto default_value;
                    }
                } else if (prev == nullptr) {
                    /** - We push a placeholder as the m_push head
                     *  so we can safely reclaim current allocated memory.
                     *  - Also ensures that there is no raise when cas:ng current with prev by reserving prev.
                     *  (if prev is null and some threading changing its value)
                     */
                    auto reserved = new NoEntry;
                    bool ret = current->previous().compare_exchange_strong(prev, reserved);
                    if (!ret) {
//                        delete reserved;
                        /** if the cas fails m_push is under contention
                         *  and we should just retry since current is likely
                         *  not the m_push head anymore.
                         */
                        goto begin;
                    } else {
                        prev = reserved;//sp
                    }
                }
                if (prev == nullptr) {
                    std::cout << "prev == nullptr\n";
                    exit(4);
                }

                bool res = m_poll.compare_exchange_strong(current, prev);
                if (!res) {
                    std::cout << "cas2\n";
                    exit(2);
//                    goto begin;
                }
                if (IEntry *e = dynamic_cast<IEntry *>(current)) {

                    T result(std::move(e->data));
//                delete current;
                    return result;
                } else {
                    //NoEntry
                    //delete current;
                }
            }
            default_value:
            return def;
        }
    };
}
#endif //PROJECT_MPSCFIFO_H

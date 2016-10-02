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

        struct Entry {
        private:
        public:
            T data;
            std::atomic<Entry *> prev;

            explicit Entry(T &&p_data) :
                    data{std::forward<T>(p_data)},
                    prev{nullptr} {

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

        using Entry_t = std::atomic<Entry *>;

        Entry_t m_push;
        Entry_t m_poll;
        Entry *m_reserved;
        //std::atomic<Entry *> m_poll;
    public:
        MpScFifo() :
                m_push{nullptr},
                m_poll{nullptr},
                m_reserved{new Entry({})} {
        }

        MpScFifo(const MpScFifo &) = delete;

        MpScFifo &operator=(const MpScFifo &) = delete;

        ~MpScFifo() {
        }

        /**
         * push a entry onto the FIFO
         * can be executed by multiple concurrent threads
         */
        template<typename D>
        void push_back(D &&data) {
            static_assert(std::is_same<typename std::remove_reference<D>::type,
                    typename std::remove_reference<T>::type>::value, "Is required to be of same type");
            auto current = new Entry(std::forward<T>(data));
            begin:
            auto head = m_push.load();
            if (head == m_reserved) {
                Entry *resrved = m_reserved;
                if (!m_push.compare_exchange_strong(resrved, current)) {
                    goto begin;
                }
                m_poll.compare_exchange_strong(resrved, current);
            } else if (head) {
                //TODO probably race
                Entry *nil = nullptr;
                if (!head->prev.compare_exchange_strong(nil, current)) {
                    goto begin;
                }

                bool res = m_push.compare_exchange_strong(head, current);
                if (!res) {
                    throw std::runtime_error{"res false"};
                }
            } else {
                Entry *nil = nullptr;
                if (!m_push.compare_exchange_strong(nil, current)) {
                    goto begin;
                }
                m_poll.compare_exchange_strong(nil, current);
            }
        }

//        size_t size() {
//            size_t cnt{0};
//            Entry_t *current = &m_push;
//            begin:
//            Entry *val = current->load();
//            if (!val) {
//                return cnt;
//            }
//            current = &val->prev;
//            cnt = cnt + 1;
//            goto begin;
//        }

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
                auto prev = current->prev.load();
                /** check if current has no parent
                 *  if so current is the head for m_push.
                 */
                if (!prev) {
                    /** we push a placeholder as the m_push head
                     *  so we can safely reclaim current allocated memory
                     */
                    bool ret = m_push.compare_exchange_strong(current, m_reserved);
                    if (!ret) {
                        /** if the cas fails m_push is under contention
                         *  and we should just retry since current is likely
                         *  not the m_push head anymore.
                         */
                        goto begin;
                    }
                }

                bool res = m_poll.compare_exchange_strong(current, prev);
                if (!res) {
                    goto begin;
                }
                T result(std::move(current->data));
                delete current;
                return result;
            }
            return def;
        }
    };
}
#endif //PROJECT_MPSCFIFO_H

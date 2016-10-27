//
// Created by spooky on 2016-10-04.
//

#ifndef PROJECT_SHAREDCONCURRENTPOINTER_H
#define PROJECT_SHAREDCONCURRENTPOINTER_H

#include <type_traits>
#include <atomic>

namespace sp {
    namespace {
        template<typename T>
        struct XEntry {
        private:
            using type = typename std::remove_pointer<T>::type;
            using pointer_type = type *;
            pointer_type m_data;
            std::atomic<int64_t> m_references;
        public:
            explicit XEntry(pointer_type data) :
                    m_data{data},
                    m_references{1} {
                std::atomic_thread_fence(std::memory_order::memory_order_release);
            }

            pointer_type data() const {
                return m_data;
            }

            void increase() {
                auto current = m_references.load();
                if (current == int64_t(0)) {
                    throw std::runtime_error("increase()");
                }
                ++m_references;
            }

            bool decrease() {
                auto current = --m_references;
                if (current == int64_t(0)) {
                    return kill();
                }
                return false;
            }

        private:
            bool kill() {
                std::atomic_thread_fence(std::memory_order::memory_order_acquire);
                if (m_data) {
                    delete m_data;
                    m_data = nullptr;
                    std::atomic_thread_fence(std::memory_order::memory_order_release);
                    return true;
                }
                return false;
            }
        };
    }
    template<typename T>
    class ConcurrentPointer {
    public:
    private:
        using type = typename std::remove_pointer<T>::type;
        using pointer_type = type *;

        XEntry<T> *m_entry;
    public:
        ConcurrentPointer() :
                m_entry(nullptr) {
        }

        ConcurrentPointer(pointer_type data) :
                m_entry(new XEntry<T>(data)) {
        }

        ConcurrentPointer(const ConcurrentPointer<T> &o) :
                m_entry{o.m_entry} {
            if (m_entry) {
                m_entry->increase();
            }
        }

        ConcurrentPointer(ConcurrentPointer<T> &&o) :
                m_entry{o.m_entry} {
            o.m_entry = nullptr;
        }

        ~ConcurrentPointer() {
            auto entry = m_entry;
            if (entry) {
                if (entry->decrease()) {
                    delete entry;
                    m_entry = nullptr;
                }
            }
        }

        ConcurrentPointer<T> &operator=(ConcurrentPointer<T> &&o) noexcept {
            swap(o);
            return *this;
        }

        ConcurrentPointer<T> &operator=(ConcurrentPointer<T> o) noexcept {
            swap(o);
            return *this;
        }

        void swap(ConcurrentPointer<T> &o) noexcept {
            std::swap(m_entry, o.m_entry);
        }

        type &operator*() {
            return *m_entry->data();
        }

//        type &operator&() {
//            return *m_entry->data();
//        }

        const type &operator*() const {
            return *m_entry->data();
        }

        pointer_type operator->() {
            return m_entry->data();
        }
    };
}

#endif //PROJECT_SHAREDCONCURRENTPOINTER_H

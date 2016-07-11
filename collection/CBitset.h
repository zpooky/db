//
// Created by spooky on 2016-04-23.
//

#ifndef PROJECT_BITSET_H
#define PROJECT_BITSET_H

#include <bitset>
#include <atomic>
#include <cmath>
#include <array>

namespace sp {
    using std::atomic;

    template<size_t T_Size>
    class CBitset {
    private:
        using Byte_t = uint8_t;
        using Entry_t =atomic<Byte_t>;
        static constexpr size_t bits = sizeof(Byte_t) * 8;
        static constexpr Byte_t one_ = 1 << (bits - 1);//10000000

        struct Entry {
        public:
            std::array<Entry_t, T_Size> m_data;

            Entry() :
                    m_data(){
            }

            explicit Entry(const std::bitset<T_Size> &init) {
                for (size_t i = 0; i < init.size(); ++i) {
                    set(i, init[i]);
                }
            }

        private:
            size_t byte_index(size_t idx) const {
                return std::ceil(double(idx) / bits);
            }

            Entry_t &word_for(size_t byteIdx) {
                return m_data[byteIdx];
            }

            const Entry_t &word_for(size_t byteIdx) const {
                return m_data[byteIdx];
            }

        public:
            using ttttt  =unsigned long long;

            bool set(size_t bitIdx, bool b) {
                size_t byteIdx = byte_index(bitIdx);
                Entry_t &e = word_for(byteIdx);

                const Byte_t offset = word_index(bitIdx);
                const Byte_t mask = one_ >> offset;

                auto word_before = e.load();
                Byte_t word;
                do {
                    if (b) {
                        word = word_before | mask;
                    } else {
                        word = word_before ^ mask;
                    }
                    //since we don't need to update if they are the same
                    if (word == word_before) {
                        // check that *word_before* has not allready been set
                        //if we did not change anything
                        return false;
                    }
                } while (!e.compare_exchange_strong(word_before, word));
                return true;
            }

            const Byte_t word_index(size_t bitIdx) const {
                return bitIdx - ttttt(ttttt(bitIdx / bits) * bits);
            }

            bool test(size_t bitIdx) const {
                size_t byteIdx = byte_index(bitIdx);
                const Entry_t &e = word_for(byteIdx);
                auto wordIdx = word_index(bitIdx);
                const Byte_t mask = one_ >> wordIdx;
                auto word = e.load();
                return word & mask != Byte_t(0);
            }
        };

        Entry *m_entry;
    private:
        explicit CBitset(Entry *e) :
                m_entry{e} {
        }

    public:
        CBitset() :
                CBitset{new Entry} {
        }

        explicit CBitset(const std::bitset<T_Size> &init) :
                CBitset{new Entry(init)} {
        }

        CBitset(const CBitset &) = delete;

        CBitset(CBitset &&o) :
                CBitset{o.m_entry} {
            o.m_entry = nullptr;
        }

        ~CBitset() {
            if (m_entry != nullptr) {
                delete m_entry;
                m_entry = nullptr;
            }
        }

        bool set(size_t bitIdx, bool b) {
            return m_entry->set(bitIdx, b);
        }

        bool test(size_t idx) const {
            return m_entry->test(idx);
        }

        bool operator[](size_t idx) const {
            return test(idx);
        }

    };

    template<size_t size>
    std::ostream &operator<<(std::ostream &os, CBitset<size> b) {

        return os;
    }
}


#endif //PROJECT_BITSET_H

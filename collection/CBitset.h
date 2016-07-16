//
// Created by spooky on 2016-04-23.
//

#ifndef PROJECT_BITSET_H
#define PROJECT_BITSET_H

#include <bitset>
#include <atomic>
#include <type_traits>
#include <cmath>
#include <array>
#include <iostream>

namespace sp {
    using std::atomic;

    template<size_t T_Size, typename Byte_t = uint8_t>
    class CBitset {
    private:
        using Entry_t =atomic<Byte_t>;
        static constexpr size_t bits = sizeof(Byte_t) * 8;
        static constexpr size_t T_Size_based_on_bits = size_t(std::ceil(double(T_Size) / bits));
        static constexpr Byte_t one_ = Byte_t(1) << size_t(bits - 1);//10000...
        //
        static_assert(std::is_scalar<Byte_t>::value, "Backing structure is required to be a scalar");
        static_assert(std::is_integral<Byte_t>::value, "Backing structure is required to be a integral");
        static_assert(T_Size % 8 == 0, "Size should be evenly divisable with 8");

        struct Entry {
        private:

        public:
            std::array<Entry_t, T_Size_based_on_bits> m_data;

            Entry() :
                    m_data() {
            }

            explicit Entry(const std::bitset<T_Size> &init) {
                transfer(init);
            }

            explicit Entry(bool v) {
                init_with(v ? ~Byte_t(0) : Byte_t(0));
            }

        private:
            constexpr size_t byte_index(size_t idx) const {
                return size_t(double(idx) / bits);
            }

            Entry_t &word_for(size_t byteIdx) {
                constexpr auto max = T_Size_based_on_bits;
                if (byteIdx >= max) {
                    throw std::out_of_range(std::string(""));
                }
                return m_data[byteIdx];
            }

            const Entry_t &word_for(size_t byteIdx) const {
                constexpr auto max = T_Size_based_on_bits;
                if (byteIdx >= max) {
                    throw std::out_of_range(std::string(""));
                }
                return m_data[byteIdx];
            }

            void store(size_t wordIdx, Byte_t value) {
                auto &entry = word_for(wordIdx);
                entry.store(value);
            }

            using ttttt  =unsigned long long;

            void init_with(Byte_t def) {
                for (size_t idx = 0; idx < T_Size_based_on_bits; ++idx) {
                    word_for(idx).store(def);
                }

            }

            void transfer(const std::bitset<T_Size> &init) {
                Byte_t word(0);
                size_t entryIdx(0);
                size_t i(0);
                for (; i < init.size(); ++i) {
                    if (init[i]) {
                        size_t bitoffset = (bits - 1) - (i % bits);
                        Byte_t cbits = Byte_t(1) << bitoffset;
                        word = word | cbits;
                    }

                    if (size_t(i + 1) % bits == size_t(0)) {
                        store(entryIdx++, word);
                        word = Byte_t(0);
                    }
                }
                if (size_t(i + 1) % bits != size_t(0)) {
                    store(entryIdx, word);
                }
            }

        public:
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

            constexpr Byte_t word_index(size_t bitIdx) const {
                return bitIdx - ttttt(ttttt(bitIdx / bits) * bits);
            }

            bool test(size_t bitIdx) const {
                size_t byteIdx = byte_index(bitIdx);
                const Entry_t &e = word_for(byteIdx);

                auto wordIdx = word_index(bitIdx);
                const Byte_t mask = one_ >> wordIdx;

                auto word = e.load();
                return Byte_t(word & mask) != Byte_t(0);
            }

            bool all(Byte_t test) const {
                for (size_t idx = 0; idx < T_Size_based_on_bits; ++idx) {
                    size_t current = m_data[idx].load();
                    if ((current & test) != test) {
                        return false;
                    }
                }
                return true;
            }

            size_t bit_index(size_t byteIdx, Byte_t wordIdx) const {
                return size_t(byteIdx * bits) + wordIdx;
            }

            size_t find_first(size_t bitIdx, bool find) const {
                size_t byteIdx = byte_index(bitIdx);
                auto wordIdx = word_index(bitIdx);

                const Byte_t mask = find ? Byte_t(0) : ~Byte_t(0);

                for (; byteIdx < T_Size_based_on_bits; ++byteIdx) {
                    const Byte_t word = word_for(byteIdx).load();

                    if (mask != word) {
                        for (size_t bit = wordIdx; bit < bits; ++bit) {

                            Byte_t vmask = one_ >> bit;
                            Byte_t cmp = find ? vmask : Byte_t(0);

                            if (Byte_t(vmask & word) == cmp) {
                                return bit_index(byteIdx, wordIdx);
                            }
                        }
                    }
                    wordIdx = Byte_t(0);
                }
                return size_t(T_Size - 1);
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

        /**
        *  @brief init the bitset with
        *  @param  b  the value to fill with
        */
        explicit CBitset(bool v)
                : CBitset(new Entry(v)) {
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

        constexpr size_t size() const {
            return T_Size;
        }

        /**
     *  @brief sets the specified bit to b
     *  @param  bitIdx  The index of a bit.
     *  @return wether b is set to b allready true, otherwise false
     *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
     */
        bool set(size_t bitIdx, bool b) {
            return m_entry->set(bitIdx, b);
        }

        bool test(size_t bitIdx) const {
            bool ret = m_entry->test(bitIdx);
            return ret;
        }

        bool operator[](size_t bitIdx) const {
            return test(bitIdx);
        }

        bool all(bool test) const {
            return m_entry->all(test ? ~Byte_t(0) : Byte_t(0));
        }

        size_t find_first(bool find) const {
            return m_entry->find_first(size_t(0), find);
        }

        size_t find_first(size_t bitIdx, bool find) const {
            return m_entry->find_first(bitIdx, find);
        }

    };


    template<size_t size, typename Type>
    std::ostream &operator<<(std::ostream &os, const CBitset<size, Type> &b) {
        for (size_t i = b.size(); i > 0; --i) {
            if (b[i - 1]) {
                std::cout << '1';
            } else {
                std::cout << '0';
            }
        }
        return os;
    }
}


#endif //PROJECT_BITSET_H

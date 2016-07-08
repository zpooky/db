//
// Created by spooky on 2016-04-23.
//

#ifndef PROJECT_BITSET_H
#define PROJECT_BITSET_H

#include <bitset>
#include <atomic>
#include <array>

namespace sp {
    template<size_t T_Size>
    class CBitset {
    private:
        using Byte_t = unsigned char;

        struct Entry {
            std::array<std::atomic<Byte_t>, T_Size> m_data;

            Entry() {
            }

            explicit Entry(const std::bitset<T_Size> &init) {

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
    };
}


#endif //PROJECT_BITSET_H

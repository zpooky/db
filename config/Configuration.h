//
// Created by spooky on 2016-03-03.
//

#ifndef DB_CONFIG_H
#define DB_CONFIG_H

#include <array>
#include "../shared/entities.h"

namespace db {
    namespace table {
        namespace name {
            constexpr size_t length = 64;
            using type = std::array<char, length>;
        }
    }
    namespace segment {
        using index_type = unsigned long;
    }
    template<typename T_table>
    struct Segment_name {
        Segment_name() {
        }

        static Filename name(segment::index_type idx) {
            char buf[table::name::length + 1 + 32];
            auto table_name = T_table::table_name();
            string name{table_name.begin(), table_name.end()};
            sprintf(buf, "%s-%ld", name.c_str(), idx);
            return Filename{buf};
        }
    };
}

#endif

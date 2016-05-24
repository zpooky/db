//
// Created by spooky on 2016-05-24.
//

#ifndef PROJECT_TABLECONTEXT_H
#define PROJECT_TABLECONTEXT_H

//#include "Assertions.h"
#include <cstddef>
#include <type_traits>

namespace db {

    template<typename T_Table>
    struct TableMeta {
        TableMeta() {
            db::assert_is_table<T_Table>();
        }

        using Table = T_Table;

        static constexpr size_t lines() {
            return 1024ul;
        }
    };

//    template<typename T>
//    void assert_is_meta();
}

//template<typename T>
//void db::assert_is_meta() {
//    static_assert(std::is_same<db::TableMeta, T>::value, "template is required to be of db::TableMeta");
//}
#endif //PROJECT_TABLECONTEXT_H

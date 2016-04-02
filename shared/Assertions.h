//
// Created by spooky on 2016-03-12.
//

#ifndef PROJECT_ASSERTIONS_H
#define PROJECT_ASSERTIONS_H

#include "fs.h"
#include <type_traits>

namespace db {
    template<typename T>
    void assert_is_table();


}

template<typename T>
void db::assert_is_table() {
    static_assert(std::is_base_of<db::Table, T>::value, "template is required to be of db::Table");
}

#endif //PROJECT_ASSERTIONS_H

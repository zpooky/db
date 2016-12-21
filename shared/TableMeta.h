//
// Created by spooky on 2016-05-24.
//

#ifndef PROJECT_TABLECONTEXT_H
#define PROJECT_TABLECONTEXT_H

#include "../page/FilePage.h"
#include <cstddef>
#include <type_traits>

namespace db {
// TODO remove HASH algh use the context one instead
template <typename T_Table, typename T_Hash_Algh>
struct TableMeta {
public:
  TableMeta() {
    db::assert_is_table<T_Table>();
  }

  using Table = T_Table;

  using hash_algh = T_Hash_Algh;
  using Page = db::fs::FilePage;

  static constexpr size_t lines() {
    return 1024ul;
  }
};

//    template<typename T>
//    void assert_is_meta();
}

// template<typename T>
// void db::assert_is_meta() {
//    static_assert(std::is_same<db::TableMeta, T>::value, "template is required
//    to be of db::TableMeta");
//}
#endif // PROJECT_TABLECONTEXT_H

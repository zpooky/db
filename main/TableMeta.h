//
// Created by spooky on 2016-05-24.
//

#ifndef PROJECT_TABLECONTEXT_H
#define PROJECT_TABLECONTEXT_H

#include <cstddef>
#include <type_traits>
#include "../page/FilePage.h"
#include "PageFileFactory.h"

namespace db {
// TODO remove HASH algh use the context one instead
template <typename Table_t, typename hash_t>
struct TableMeta {
public:
  TableMeta() {
    db::assert_is_table<Table_t>();
  }

  using Table = Table_t;

  using hash_algh = hash_t;

  static constexpr size_t extent_lines() {
    return 1024ul;
  }
  /**
   * Version of this table structure
   */
  constexpr static db::table::version latest_version = 1;
  /**
   * Tablename
   */
  constexpr static db::table::name::type table_name() {
    //        return to_array("test_table");
    return {'t', 'e', 's', 't', '_', 't', 'a', 'b', 'l', 'e', 0};
  }

  using Page = page::FilePage<TableMeta<Table_t,hash_t>>;
  using PageFactory = page::PageFileFactory<TableMeta<Table_t,hash_t>>;
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

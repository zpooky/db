//
// Created by spooky on 2016-03-12.
//

#ifndef PROJECT_ASSERTIONS_H
#define PROJECT_ASSERTIONS_H

#include "../shared/shared.h"
#include <type_traits>

namespace db {
template <typename T>
void assert_is_table();
}

template <typename T>
void db::assert_is_table() {
  // static_assert(
  //     std::is_same<decltype(T::table_name()), db::table::name::type>::value,
  //     "table should implement 'static constexpr table_name()'");
}

#endif // PROJECT_ASSERTIONS_H

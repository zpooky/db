//
// Created by spooky on 2016-03-03.
//

#ifndef _SP_FS_LINE_H
#define _SP_FS_LINE_H

#include "../shared/Assertions.h"
#include "../shared/Buffer.h"
#include "../shared/shared.h"
#include "../segment/Context.h"
#include <iostream>
#include <stddef.h>
#include <type_traits>
#include <utility>

namespace db {
namespace fs {
template <size_t T_bytes, typename hash_t>
struct Line {
private:
  using raw_type_t = db::raw::type<T_bytes>;

public:
  const db::raw::id id;
  //            T_hash_type checksum;
  raw_type_t data;

  explicit Line(Table &&table) : id(1) {
  }

  template <size_t bytes>
  explicit Line(Buffer<bytes> &buf) : id(Context<hash_t>::endianess::get_uint64(buf)) {
  }

  static constexpr size_t bytes() {
    return T_bytes;
  }

private:
  static constexpr size_t multipleOf(size_t size, size_t multiple) {
    return size < multiple ? multiple : multipleOf(size, multiple * 2);
  }

  static constexpr size_t multipleOf(size_t size) {
    return multipleOf(size, 8);
  }

public:
  static constexpr size_t size() {
    return multipleOf(
        sizeof(db::raw::id) +
        /* sizeof(T_hash_type) */ sizeof(raw_type_t));
  }
};

template <typename T_Table>
struct Table_size {
private:
public:
  Table_size() {
    db::assert_is_table<T_Table>();
  }

  static constexpr size_t value() {
    return sizeof(T_Table);
  }
};

template <typename T_Meta>
struct Line_size {
private:
  using T_Table = typename T_Meta::Table;
  using hash_t = typename T_Meta::hash_algh;

public:
  Line_size() {
    db::assert_is_table<T_Table>();
  }

  static constexpr size_t value() {
    return Line<Table_size<T_Table>::value(), hash_t>::size();
  }
};

template <size_t LINE_SIZE, typename hash_t>
auto buffer(const Line<LINE_SIZE, hash_t> &);

template <typename T_Table, typename hash_t>
auto to_line(T_Table &&table) -> Line<sizeof(T_Table), hash_t>;

template <size_t LINE_SIZE, typename hash_t>
auto buffer(const Line<LINE_SIZE, hash_t> &l) {
  Buffer<Line<LINE_SIZE, hash_t>::size()> buf;
  Context<hash_t>::endianess::put(buf, l.id);
  //            buf.put(l.checksum);
  //            auto state = static_cast<unsigned char>(l.state);
  //            buf.put(state);
  buf.put(l.data);
  return buf;
}

template <typename T_Table, typename hash_t>
auto to_line(T_Table &&table) -> Line<sizeof(T_Table), hash_t> {
  db::assert_is_table<T_Table>();
  std::cout << "sizeof:" << sizeof(T_Table) << "\n";
  //            sizeof(std::remove_const<std::remove_reference<std::decay<decltype(table)>::type>::type>::type)
  //            << "\n";
  Line<sizeof(table), hash_t> line{std::forward<T_Table>(table)};
  return line;
}
}
}

#endif // FS_LINE_H

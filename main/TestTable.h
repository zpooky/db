#ifndef PROJECT_TESTTABLE_H
#define PROJECT_TESTTABLE_H

#include "../page/FilePage.h"
#include "PageFileFactory.h"
#include "../shared/shared.h"
#include <array>
#include "../shared/hash.h"

struct TestTable {
private:
  static constexpr size_t data_size = 5;
  using data_t = std::array<char, data_size>;
  using id_t = uint64_t;

public:
  /**
   * Version of this table structure
   */
  constexpr static db::table::version version = 1;

public:
  const data_t data;
  const id_t id;

  TestTable() : data{"hell"}, id{1} {
  }

  TestTable(data_t p_data, id_t p_id) : data{p_data}, id{p_id} {
  }

public:
  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const {
    Endianess::put_arr(b, data);
    Endianess::put(b, id);
  }

  static constexpr db::raw::size size() {
    return sizeof(data_t) + sizeof(id_t);
  }

  template <typename Endianess, typename Buffer>
  static auto read(Buffer &b) {
    auto data =
        Endianess::template read_arr<Buffer, data_t::value_type, data_size>(b);
    auto id = Endianess::template read<Buffer, id_t>(b);
    return TestTable{data, id};
  }
};

struct TestTableMetax {
  using latest = TestTable;

  constexpr static db::table::name::type table_name() {
    //        return to_array("test_table");
    return {'t', 'e', 's', 't', '_', 't', 'a', 'b', 'l', 'e', 0};
  }

  using hash_t = sp::crc32;

  static constexpr size_t extent_lines() {
    return 1024ul;
  }

  using Page = page::FilePage<TestTableMetax>;
  using PageFactory = page::PageFileFactory<TestTableMetax>;
};

struct Test2Table {
private:
  static constexpr size_t dsize = 7;
  using data_t = std::array<char, dsize>;
  using id_t = uint64_t;

public:
  /**
   * Version of this table structure
   */
  constexpr static db::table::version version = 1;

public:
  const data_t data;
  const data_t data2;
  const id_t id;

  Test2Table() : data{"sPooKy"}, data2{"WaSdAx"}, id{1} {
  }

  Test2Table(id_t p_id, data_t p_data, data_t p_data2)
      : data{p_data}, data2{p_data2}, id{p_id} {
  }

public:
  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const {
    Endianess::put(b, id);
    Endianess::put_arr(b, data);
    Endianess::put_arr(b, data2);
  }

  static constexpr db::raw::size size() {
    return sizeof(data_t) + sizeof(id_t) + sizeof(data_t);
  }

  template <typename Endianess, typename Buffer>
  static auto read(Buffer &b) {
    using vt = typename data_t::value_type;

    auto id = Endianess::template read<Buffer, id_t>(b);
    auto data = Endianess::template read_arr<Buffer, vt, dsize>(b);
    auto data2 = Endianess::template read_arr<Buffer, vt, dsize>(b);
    return Test2Table{id, data, data2};
  }
};

struct Test2TableMetax {
  using latest = Test2Table;

  constexpr static db::table::name::type table_name() {
    return {'t', 'e', 's', 't', '2', '_', 't', 'a', 'b', 'l', 'e', 0};
  }

  using hash_t = sp::crc32;

  static constexpr size_t extent_lines() {
    return 1024ul;
  }

  using Page = page::FilePage<Test2TableMetax>;
  using PageFactory = page::PageFileFactory<Test2TableMetax>;
};

#endif // PROJECT_TESTTABLE_H

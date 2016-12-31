#ifndef PROJECT_TESTTABLE_H
#define PROJECT_TESTTABLE_H

#include "../shared/shared.h"
#include <array>

struct TestTable : public db::Table {
private:
  using data_t = std::array<char, 5>;
  using id_t = uint64_t;

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
  static TestTable read(Buffer &b) {
    auto data = Endianess::template read_arr<Buffer, data_t::value_type, 5>(b);
    auto id = Endianess::template read<Buffer, id_t>(b);
    return TestTable{data, id};
  }
};

#endif // PROJECT_TESTTABLE_H

#include "../../shared/Buffer.h"
#include "../../shared/LittleEndian.h"
#include "../Line.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace db;
namespace {
struct StructLineTest {
  using data_t = std::array<char, 5>;
  using id_t = uint64_t;

  const data_t data;
  const id_t id;

  StructLineTest(data_t p_data, id_t p_id) : data{p_data}, id{p_id} {
  }

public:
  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const {
    Endianess::put(b, id);
    Endianess::put_arr(b, data);
  }

  static constexpr db::raw::size size() {
    return sizeof(data_t) + sizeof(id_t);
  }

  template <typename Endianess, typename Buffer>
  static auto read(Buffer &b) {
    auto id = Endianess::template read<Buffer, id_t>(b);
    auto data = Endianess::template read_arr<Buffer, data_t::value_type, 5>(b);
    return StructLineTest{data, id};
  }
};
}

TEST(LineTest, test) {
  using Table_t = StructLineTest;
  using Line_t = Line<Table_t>;
  using data_t = StructLineTest::data_t;
  using Buffer_t = db::Buffer<Line_t::size()>;

  db::raw::id id(~0);
  db::raw::version_t v(1294394994);
  Table_t t(data_t{"wasd"}, 1337ul);
  Line_t line(db::LineMeta(id, v), t);
  Buffer_t b;

  line.write<db::LittleEndian, Buffer_t>(b);
  ASSERT_EQ(b.capacity(), Line_t::size());
  ASSERT_EQ(b.position(), Line_t::size());

  b.flip();
  ASSERT_EQ(b.size(), Line_t::size());
  ASSERT_EQ(b.capacity(), Line_t::size());
  ASSERT_EQ(b.position(), size_t(0));

  Line_t read = line.read<db::LittleEndian, Buffer_t>(b);
  ASSERT_EQ(read.meta.id, line.meta.id);
  ASSERT_EQ(read.meta.version, line.meta.version);
  auto readData = read.data;
  ASSERT_EQ(readData.data, t.data);
  ASSERT_EQ(readData.id, t.id);

  ASSERT_EQ(b.size(), Line_t::size());
  ASSERT_EQ(b.capacity(), Line_t::size());
  ASSERT_EQ(b.position(), b.size());
}

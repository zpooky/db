#ifndef _SP_FS_LINE_H
#define _SP_FS_LINE_H

#include "../shared/shared.h"

namespace db {

// TODO maybe do not copy Table_t data
template <typename Table_t, typename hash_t>
class Line {
public:
  const db::raw::id id;
  const db::raw::version_t version;
  const Table_t data;

  explicit Line(db::raw::id p_id, db::raw::version_t p_v, const Table_t &table)
      : id(p_id), version(p_v), data(table) {
  }

  explicit Line(db::raw::id p_id, db::raw::version_t p_v, const Table_t &&table)
      : id(p_id), version(p_v), data(std::move(table)) {
  }

  Line(Line<Table_t, hash_t> &&o) : id(o.id), version(o.version), data(o.data) {
  }

  Line(Line<Table_t, hash_t> &) = delete;

  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const noexcept {
    Endianess::put(b, id);
    Endianess::put(b, version);
    data.write<Endianess, Buffer>(b);
  }

  // TODO allign with 8
  static constexpr db::raw::size size() {
    return sizeof(db::raw::id) + sizeof(db::raw::version_t) + Table_t::size();
  }

  // TODO endianess should be base on the file no determined in compile time
  template <typename Endianess, typename Buffer>
  static Line<Table_t, hash_t> read(Buffer &b) {
    auto id = Endianess::template read<Buffer, db::raw::id>(b);
    auto v = Endianess::template read<Buffer, db::raw::version_t>(b);
    auto data = Table_t::template read<Endianess, Buffer>(b);
    return Line<Table_t, hash_t>(id, v, data);
  }

private:
  // static constexpr size_t multipleOf(size_t size, size_t multiple) {
  //   return size < multiple ? multiple : multipleOf(size, multiple * 2);
  // }
  //
  // static constexpr size_t multipleOf(size_t size) {
  //   return multipleOf(size, 8);
  // }

public:
  // static constexpr size_t size() {
  //   return multipleOf(sizeof(db::raw::id) +
  //                     #<{(| sizeof(T_hash_type) |)}># sizeof(raw_type_t));
  // }
};
}

#endif // FS_LINE_H

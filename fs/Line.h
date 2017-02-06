#ifndef _SP_FS_LINE_H
#define _SP_FS_LINE_H

#include "../shared/shared.h"

namespace db {
  //TODO move to shared - since it is used by tx which should not know about "fs"
struct LineMeta {
  const db::raw::id id;
  const db::raw::version_t version;

  explicit LineMeta(db::raw::id p_id, db::raw::version_t p_v)
      : id(p_id), version(p_v) {
  }

  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const {
    Endianess::put(b, id);
    Endianess::put(b, version);
  }

  template <typename Endianess, typename Buffer>
  static LineMeta read(Buffer &b) {
    auto id = Endianess::template read<Buffer, db::raw::id>(b);
    auto v = Endianess::template read<Buffer, db::raw::version_t>(b);
    return LineMeta(id, v);
  }

  operator bool() const {
    return id != db::raw::EMPTY;
  }
};
// TODO maybe do not copy Table_t data
template <typename Table_t>
class Line {
public:
  const LineMeta meta;
  const Table_t data;

  explicit Line(const LineMeta &m, const Table_t &table)
      : meta{m}, data(table) {
  }

  explicit Line(const LineMeta &m, Table_t &&table)
      : meta{m}, data(std::move(table)) {
  }

  Line(Line<Table_t> &&o)
      : meta{std::move(o.meta)}, data(std::move(o.data)) {
  }

  Line(Line<Table_t> &) = delete;

  template <typename Endianess, typename Buffer>
  void write(Buffer &b) const {
    meta.write<Endianess, Buffer>(b);
    data.write<Endianess, Buffer>(b);
  }

  // TODO allign with 8
  static constexpr db::raw::size size() {
    return sizeof(db::raw::id) + sizeof(db::raw::version_t) + Table_t::size();
  }

  // TODO endianess should be base on the file no determined in compile time
  template <typename Endianess, typename Buffer>
  static Line<Table_t> read(Buffer &b) {
    auto meta = db::LineMeta::read<Endianess, Buffer>(b);
    auto data = Table_t::template read<Endianess, Buffer>(b);
    return Line<Table_t>(meta, data);
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

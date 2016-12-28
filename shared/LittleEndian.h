#ifndef PROJECT_LITTLE_ENDIAN_H
#define PROJECT_LITTLE_ENDIAN_H

#include "Buffer.h"
#include <type_traits>

namespace db {
class LittleEndian {
private:
  template <typename In>
  static uint8_t to_uint8(In datum, size_t index) {
    uint8_t shift(index * 8);
    // std::cout << "x >> " << size_t(shift) << "# " << size_t(uint8_t(datum >>
    // bits));

    return uint8_t(datum >> shift);
  }

  static uint8_t to_uint8(uint8_t datum, size_t) {
    return datum;
  }

  template <typename In, typename Buff>
  static void put_req(size_t index, In in, Buff &b) {
    /**
     * put the most right byte first in the buffer
     */
    // std::cout << "_put(a + " << index << ", long" << index << "(x); # ";
    b.put(to_uint8<In>(in, index));
    // std::cout << std::endl;
    if (index != sizeof(In) - 1) {
      put_req<In, Buff>(index + 1, in, b);
    }
  }

  template <typename datum_t, typename Buff>
  static datum_t get(Buff &b) {
    constexpr size_t bytes(sizeof(datum_t) / sizeof(uint8_t));
    std::array<uint8_t, bytes> buff;
    b.get(buff);
    datum_t res(0);
    // std::cout << "makeLong(";
    for (size_t i = 0; i < bytes; ++i) {
      // std::cout << "\tget(a + " << i << ") << " << (i) << ",# " <<
      // size_t(buff[i])<<std::endl;
      res = res | (datum_t(buff[i]) << (i * 8));
      // std::cout << "";
    }
    // std::cout << ")" << std::endl;
    return res;
  }

public:
  template <typename Buff, typename datum_t>
  static void put(Buff &b, datum_t datum) {
    static_assert(std::is_integral<datum_t>::value, "");
    put_req<datum_t, Buff>(0, datum, b);
  }

  template <typename Buff, typename datum_t, size_t size>
  static void put_arr(Buff &b, const std::array<datum_t, size> &datum) {
    for (size_t i(0); i < size; ++i) {
      put(b, datum[i]);
    }
  }

  template <typename Buff, typename datum_t>
  static datum_t read(Buff &b) {
    static_assert(std::is_integral<datum_t>::value, "");
    return get<datum_t, Buff>(b);
  }

  template <typename Buff, typename datum_t, size_t size>
  static std::array<datum_t, size> read_arr(Buff &b) {
    std::array<datum_t, size> res{};
    for (size_t i(0); i < size; ++i) {
      res[i] = read<Buff, datum_t>(b);
    }
    return res;
  }
};
}
#endif

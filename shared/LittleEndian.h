#ifndef PROJECT_LITTLE_ENDIAN_H
#define PROJECT_LITTLE_ENDIAN_H

#include "Buffer.h"

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

public:
  template <typename T, typename Buff>
  static T get(Buff &b) {
    constexpr size_t bytes(sizeof(T) / sizeof(uint8_t));
    std::array<uint8_t, bytes> buff;
    b.get(buff);
    T res(0);
    // std::cout << "makeLong(";
    for (size_t i = 0; i < bytes; ++i) {
      // std::cout << "\tget(a + " << i << ") << " << (i) << ",# " <<
      // size_t(buff[i])<<std::endl;
      res = res | (T(buff[i]) << (i * 8));
      // std::cout << "";
    }
    // std::cout << ")" << std::endl;
    return res;
  }

  template <typename Buff>
  static void put(Buff &b, uint64_t datum) {
    put_req<uint64_t, Buff>(0, datum, b);
  }

  template <typename Buff>
  static uint64_t get_uint64(Buff &b) {
    return get<uint64_t, Buff>(b);
  }
};
}
#endif

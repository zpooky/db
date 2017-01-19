//
// Created by spooky on 2016-09-25.
//

#ifndef PROJECT_BASE_H
#define PROJECT_BASE_H

#include <array>
#include <stddef.h>
#include <vector>

namespace sp {
namespace {
/* bits required to represent T_Radix
 */
template <size_t T_Radix>
constexpr size_t xxbits(/*size_t ret = 1*/) {
  //            return (T_Radix & 1) & 1 ? xxbits<size_t(T_Radix >> 1)>(ret + 1)
  //            : ret;
  if (T_Radix == 16) {
    return 4;
  } else if (T_Radix == 64) {
    return 6;
  } else if (T_Radix == 32) {
    return 5;
  }
  throw std::runtime_error("bits");
}

template <size_t T_Radix>
constexpr uint64_t xmask() {
  return T_Radix - 1;
}

template <size_t T_Radix>
struct Encoder {
private:
  static constexpr size_t size = sizeof(uint8_t) * 8;
  uint64_t left;
  size_t bits;
  const std::array<uint8_t, T_Radix> &m_pool;

  constexpr size_t xbits() {
    return xxbits<T_Radix>();
  }

  constexpr uint64_t mask() const {
    return xmask<T_Radix>();
  }

public:
  constexpr Encoder(uint64_t p_left, size_t p_bits,
                    const std::array<uint8_t, T_Radix> &pool)
      : left(p_left), bits(p_bits), m_pool(pool) {
  }

  Encoder<T_Radix> &operator=(Encoder<T_Radix> e) {
    std::swap(left, e.left);
    std::swap(bits, e.bits);
    return *this;
  }

  Encoder<T_Radix> add(uint8_t data) {
    uint64_t c(uint64_t(left << size) | data);
    return {c, bits + size, m_pool};
  }

  template <typename Result>
  Encoder<T_Radix> encode(Result &res) {
    if (bits < xbits()) {
      throw std::runtime_error("bah");
    }

    auto msk = mask();
    uint64_t rsft(bits - xbits());
    uint64_t c(left >> rsft);
    size_t idx = msk & c;
    res.push_back(m_pool[idx]);

    return {left, bits - xbits(), m_pool};
  }

  bool fitting() {
    return bits >= xbits();
  }

  template <typename Result>
  Encoder<T_Radix> encode_padding(char pad, Result &res) {
    if (bits != 0) {
      // TODO assert bits <=
      const auto msk = mask();

      // shift to get the zeros on the right
      uint64_t c((left << (xbits() - bits)));
      size_t idx = msk & c;
      res.push_back(m_pool[idx]);

      // add padding
      const size_t no_pad(padding(xbits() - bits, xbits()));
      for (size_t i(0); i < no_pad; ++i) {
        res.push_back(pad);
      }

      return {left, 0, m_pool};
    }
    return *this;
  }

  size_t padding(size_t bits, size_t radix) {
    size_t pad(bits);
    size_t result(0);
    while (pad % size != 0) {
      pad = (pad + radix);
      result = result + 1;
    }
    return result;
  }
};

template <size_t T_Radix>
struct Decoder {
private:
  static constexpr size_t size = sizeof(uint8_t) * 8;
  uint64_t left;
  size_t bits;
  const std::array<uint8_t, T_Radix> &m_pool;
  const uint8_t m_pad;

  constexpr size_t xbits() {
    return xxbits<T_Radix>();
  }

  size_t convert(uint8_t in) const {
    // TODO improve
    size_t i(0);
    for (auto cur : m_pool) {
      if (in == cur) {
        return i;
      }
      i = i + 1;
    }
    throw std::runtime_error("convert");
  }

public:
  constexpr Decoder(uint64_t p_left, size_t p_bits,
                    const std::array<uint8_t, T_Radix> &pool, uint8_t pad)
      : left(p_left), bits(p_bits), m_pool(pool), m_pad(pad) {
  }

  void add(uint8_t in) {
    if (in != m_pad) {
      auto data = convert(in);
      auto bb = xbits();
      left = uint64_t(left << bb) | uint64_t(data);
      bits = bits + bb;
    }
  }

private:
  uint8_t get() {
    bits = std::max(int32_t(bits - size), int32_t(0));
    return uint8_t(uint64_t(left >> bits) & 0xFF);
  }

  constexpr size_t cnt() {
    return bits / size;
  }

public:
  template <typename Inserter>
  void decode(Inserter &insert) {
    for (size_t i(0); i < cnt(); ++i) {
      insert = get();
    }
  }

  template <typename Inserter>
  void decode_last(Inserter &insert) {
    // TODO ????
    //                while (bits > 0) {
    //                    insert = get();
    //                }
  }

public:
  bool fitting() {
    return bits >= size;
  }
};
}
template <size_t T_Radix>
class Base {
private:
  const std::array<uint8_t, T_Radix> m_pool;
  const uint8_t m_pad;

private:
public:
  static_assert(T_Radix % 8 == 0, "");
  static_assert(T_Radix > 0, "");
  static_assert(T_Radix <= size_t(64), "");

  constexpr Base(std::array<uint8_t, T_Radix> &&pool, uint8_t pad)
      : m_pool(pool), m_pad{pad} {
  }

  template <size_t T_In>
  std::string encode(const std::array<uint8_t, T_In> &in) const {
    std::string res;
    return encode(in.begin(), in.end(), res);
  }

  std::string encode(const std::string &in) const {
    auto begin = in.c_str();
    auto end = &begin[in.length()];
    std::string res;
    return encode(begin, end, res);
  }

  template <typename Result>
  Result decode(const std::string &in) const {
    Result v /*(in.length())*/;
    decode(in, std::back_inserter(v));
    return v;
  }

  template <typename Inserter>
  void decode(const std::string &in, Inserter insert) const {
    auto begin = in.c_str();
    auto end = &begin[in.length()];
    return decode(begin, end, insert);
  }

private:
  template <typename Char_t, typename Result>
  Result &encode(const Char_t *begin, const Char_t *end, Result &res) const {
    Encoder<T_Radix> left(0, 0, m_pool);
    for (; begin != end; ++begin) {
      left = left.add(*begin);
      while (left.fitting()) {
        left = left.encode(res);
      }
    }
    left.encode_padding(m_pad, res);
    return res;
  }

  template <typename Char_t, typename Inserter>
  void decode(const Char_t *begin, const Char_t *end, Inserter &insert) const {
    Decoder<T_Radix> left(0, 0, m_pool, m_pad);
    for (; begin != end; ++begin) {
      left.add(*begin);
      while (left.fitting()) {
        left.decode(insert);
      }
      left.decode_last(insert);
    }
  }
};

using Hex = Base<16>;

constexpr Hex hex() {
  using arr = std::array<uint8_t, 16>;
  return Hex{arr{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
                 'C', 'D', 'E', 'F'},
             0};
}

using Base32 = Base<32>;

constexpr Base32 base32() {
  std::array<uint8_t, 32> pool{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                               'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                               'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                               'Y', 'Z', '2', '3', '4', '5', '6', '7'};
  Base32 b{std::move(pool), '='};
  return b;
}

using Base64 = Base<64>;

constexpr Base64 base64() {
  std::array<uint8_t, 64> pool{
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
  Base64 b{std::move(pool), '='};
  return b;
}
}

#endif // PROJECT_BASE_H

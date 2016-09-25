//
// Created by spooky on 2016-09-25.
//

#ifndef PROJECT_BASE_H
#define PROJECT_BASE_H

#include <stddef.h>
#include <array>

namespace sp {
    template<size_t T_bits>
    class Base {
    private:
        static constexpr size_t size = sizeof(uint8_t) * 8;
        const std::array<uint8_t, T_bits> m_pool;
        const uint8_t m_pad;
    private:

    public:
        static_assert(T_bits % 8 == 0, "");
        static_assert(T_bits > 0, "");
        static_assert(T_bits <= size_t(64), "");

        constexpr Base(std::array<uint8_t, T_bits> &&pool, uint8_t pad) :
                m_pool(pool), m_pad{pad} {
        }

        template<size_t T_In>
        std::string encode(const std::array<uint8_t, T_In> &in) const {
            return encode(in.begin(), in.end());
        }

        std::string encode(const std::string &in) const {
            auto begin = in.c_str();
            auto end = &begin[in.length()];
            return encode(begin, end);
        }

        struct Entry {
        private:
            uint64_t left;
            size_t bits;
            const std::array<uint8_t, T_bits> &m_pool;

            constexpr uint64_t mask() const {
                return T_bits - 1;
            }

            constexpr uint8_t xbits() const {
                if (T_bits == 16) {
                    return 4;
                } else if (T_bits == 64) {
                    return 6;
                } else if (T_bits == 32) {
                    return 5;
                }
                throw std::runtime_error("bits");
            }

        public:
            Entry(uint64_t p_left, size_t p_bits, const std::array<uint8_t, T_bits> &pool) :
                    left(p_left), bits(p_bits), m_pool(pool) {
            }

            Entry &operator=(Entry e) {
                std::swap(left, e.left);
                std::swap(bits, e.bits);
                return *this;
            }

            Entry add(uint8_t data) {
                uint64_t c(uint64_t(left << size) | data);
                return {c, bits + size, m_pool};
            }

            Entry word(std::string &res) {
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

            Entry last_padding(char pad, std::string &res) {
                if (bits != 0) {
                    //TODO assert bits <=
                    const auto msk = mask();

                    //shift to get the zeros on the right
                    uint64_t c((left << (xbits() - bits)));
                    size_t idx = msk & c;
                    res.push_back(m_pool[idx]);

                    //add padding
                    const size_t no_pad(padding(xbits() - bits, xbits()));
                    for (size_t i(0); i < no_pad; ++i) {
                        res.push_back(pad);
                    }

                    return {left, 0, m_pool};
                }
                return *this;
            }

        private:
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

        template<typename Char_t>
        std::string encode(const Char_t *begin, const Char_t *end) const {
            std::string res;
            Entry left(0, 0, m_pool);
            for (; begin != end; ++begin) {
                left = left.add(*begin);
                while (left.fitting()) {
                    left = left.word(res);
                }
            }
            left.last_padding(m_pad, res);
            return res;
        }
    };

    using Hex = Base<16>;

    constexpr Hex hex() {
        using arr = std::array<uint8_t, 16>;
        return Hex{arr{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}, ' '};
    }

    using Base32 = Base<32>;

    constexpr Base32 base32() {
        std::array<uint8_t, 32> pool{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'};
        Base32 b{std::move(pool), '='};
        return b;
    }

    using Base64 = Base<64>;

    constexpr Base64 base64() {
        std::array<uint8_t, 64> pool{
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+',
                '/'
        };
        Base64 b{std::move(pool), '='};
        return b;
    }
}

#endif //PROJECT_BASE_H

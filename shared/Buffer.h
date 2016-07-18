//
// Created by spooky on 2016-03-09.
//

#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H


#include <glob.h>
#include <array>
#include <iostream>

namespace db {
    using std::array;

    template<size_t>
    class Buffer;
    namespace be {
        namespace internal {


//            template<typename In, typename Buff>
//            void put_req(size_t index, In in, Buff &b) {
//                /**
//                 * put the most left byte first in the buffer
//                 */
//                b.put(to_uint8<In>(in, index));
//                if (index != 0) {
//                    put_req<In, Buff>(index - 1, in, b);
//                }
//            }
        }
    }
    /**
     * Little Endian
     *
     * 10110011_10001111_00001111_10000011_11110000_00111111_10000000_11111111
     * ^ least significant                                  most significant ^
     * >------------------------------->
     */
    namespace le {
        namespace internal {
            template<typename In>
            uint8_t to_uint8(In datum, size_t index) {
                uint8_t shift(index * 8);
                //std::cout << "x >> " << size_t(shift) << "# " << size_t(uint8_t(datum >> shift));

                return uint8_t(datum >> shift);
            }

            template<size_t, typename Buff, typename In>
            void put_req(size_t, In, Buff &);

            template<typename T, typename Buff>
            T get(Buff &);

            template<typename In, typename Buff>
            void put_req(size_t index, In in, Buff &b) {
                /**
                 * put the most right byte first in the buffer
                 */
                //std::cout << "_put(a + " << index << ", long" << index << "(x); # ";
                b.put(to_uint8<In>(in, index));
                //std::cout << std::endl;
                if (index != sizeof(In) - 1) {
                    put_req<In, Buff>(index + 1, in, b);
                }
            }


            template<typename T, typename Buff>
            T get(Buff &b) {
                constexpr size_t bytes(sizeof(T) / sizeof(uint8_t));
                std::array<uint8_t, bytes> buff;
                b.get(buff);
                T res(0);
                //std::cout << "makeLong(";
                for (size_t i = 0; i < bytes; ++i) {
                    //std::cout << "\tget(a + " << i << ") << " << (i) << ",# " << size_t(buff[i])<<std::endl;
                    res = res | (T(buff[i]) << (i * 8));
                    //std::cout << "";
                }
                //std::cout << ")" << std::endl;
                return res;
            }
        }

        template<typename Buff>
        void put(Buff &, uint64_t);


        template<typename Buff>
        void put(Buff &b, uint64_t datum) {
            internal::put_req<uint64_t, Buff>(0, datum, b);
        }


        template<typename Buff>
        uint64_t get_uint64(Buff &b) {
            return internal::get<uint64_t, Buff>(b);
        }

    }
    template<size_t t_capacity>
    class Buffer {
    private:
        using Arr = array<uint8_t, t_capacity>;
        Arr m_data;
        size_t m_position;
        size_t m_size;
    public:
        explicit Buffer(Arr &&data) :
                m_data(std::move(data)),
                m_position{0},
                m_size{0} {
        }

        Buffer() : Buffer(Arr{0}) {

        }

        Buffer(const Buffer &) = delete;

        Buffer(Buffer &&o) :
                m_data(std::move(o.m_data)),
                m_position(o.m_position),
                m_size(o.m_size) {

        }

    private:
        void i_put(uint8_t datum) {
            m_data[m_position++] = datum;
            ++m_size;
        }

        template<size_t bytes>
        void i_get(std::array<uint8_t, bytes> &buff) {
            for (size_t i = 0; i < buff.size(); ++i) {
                buff[i] = m_data[m_position++];
            }
        }

    public:
        size_t postion() const {
            return m_position;
        }

        size_t size() const {
            return m_size;
        }

        void flip() {
            m_position = size_t(0);
        }

        void clear() {
            flip();
            m_size = size_t(0);
        }

        void put(uint8_t datum) {
            if (postion() >= size()) {
                throw std::runtime_error("put position() >= size()");
            }
            i_put(datum);
        }

        void put(const uint8_t *datum, size_t length) {
            if (postion() + length >= size()) {
                throw std::runtime_error("put position()+length >= size()");
            }
            auto begin = &datum[0];
            auto end = begin + length;
            for (auto it = begin; it != end; ++it) {
                i_put(*it);
            }
        }

        template<size_t size>
        void put(const uint8_t (&datum)[size]) {
            put(datum, size);
        }

        template<size_t size>
        void put(const array<uint8_t, size> &datum) {
            put(&datum.front(), size);
        }


//        void put(uint32_t datum) {
//
//        }

        uint8_t get() {
            if (postion() >= size()) {
                throw std::runtime_error("get position() >= size()");
            }
            return m_data[m_position++];
        }

        template<size_t bytes>
        void get(std::array<uint8_t, bytes> &buff) {
            if (postion() + bytes >= size()) {
                throw std::runtime_error("get position() + bytes >= size()");
            }
            i_get(buff);
        }

        uint8_t *writable_data() {
            return m_data.data() + postion();
        }

        const uint8_t *full_data() const {
            return m_data.data();
        }

        void position(size_t pos) {
            m_position = pos;
            if (postion() > size()) {
                m_size = pos;
            }
        }

        constexpr size_t capacity() const {
            return t_capacity;
        }

    };

}


#endif //PROJECT_BUFFER_H

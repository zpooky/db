//
// Created by spooky on 2016-03-09.
//

#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H


#include <glob.h>
#include <array>


namespace db {
    namespace fs {
        using std::array;

        template<size_t t_capacity>
        class Buffer {
        private:
            using Arr = array<char, t_capacity>;
            Arr m_data;
            size_t m_position;
            size_t m_size;
        public:
            explicit Buffer(Arr &&data) :
                    m_data(std::move(data)) {
            }

            Buffer() : m_position{0}, m_size{0} {

            }

            void put(const unsigned char datum[], size_t length) {
                auto begin = &datum[0];
                for (auto it = begin; it != begin + length; ++it) {
                    put(*it);
                }
            }

            template<size_t size>
            void put(const unsigned char (&datum)[size]) {
                put(datum, size);
            }

            template<size_t size>
            void put(const array<unsigned char, size> &datum) {
                for (const auto &d : datum) {
                    put(d);
                }
            }


            void put(unsigned long) {

            }

            void put(unsigned char datum) {
                m_data[m_position++] = datum;
                ++m_size;
            }

            void put(unsigned datum) {

            }

            void *data() {
                return static_cast< void *>(m_data.data());
            }

            const void *data() const {
                return static_cast< void *>(m_data.data());
            }

            constexpr size_t capacity() const {
                return t_capacity;
            }

        };

    }
}


#endif //PROJECT_BUFFER_H

//
// Created by spooky on 2016-03-09.
//

#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H


#include <glob.h>
#include <array>
#include "Line.h"

using std::array;

namespace db {
    namespace fs {
        template<size_t size>
        class Buffer {
        private:
            array<char, size> m_data;
        public:


            Buffer() {
            }

            Buffer(array<char, size> data) : m_data{data} {

            }

            void *data() {
                return static_cast< void *>(m_data.data());
            }

            constexpr size_t length() const {
                return size;
            }

        };

        template<size_t LINE_SIZE>
        auto buffer(const Line <LINE_SIZE> &l) {
            Buffer<Line<LINE_SIZE>::size()> buf{};
            return buf;
        }
    }
}


#endif //PROJECT_BUFFER_H

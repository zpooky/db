//
// Created by spooky on 2016-03-03.
//

#ifndef _SP_FS_LINE_H
#define _SP_FS_LINE_H

#include <stddef.h>
#include "../shared/fs.h"
#include <type_traits>
#include <utility>
#include "../shared/Buffer.h"
#include "../shared/Assertions.h"
#include <iostream>

namespace db {
    namespace fs {
        template<size_t T_bytes, typename T_hash_type = std::array<unsigned char, 4>>//TODO make hash_type more dynamic
        struct Line {
        public:
            db::rid id;
            T_hash_type checksum;
            db::RState state;
            db::raw<T_bytes> data;

            explicit Line(Table &&table) {
                id = 1;

            }

            static constexpr size_t bytes() {
                return T_bytes;
            }
        private:
            static constexpr size_t multipleOf(size_t size, size_t multiple) {
                return size < multiple ? multiple : multipleOf(size, multiple * 2);
            }

            static constexpr size_t multipleOf(size_t size) {
                return multipleOf(size, 8);
            }
        public:

            static constexpr size_t size() {
                return multipleOf(sizeof(db::rid) + sizeof(T_hash_type) + sizeof(db::RState) + sizeof(db::raw<T_bytes>));
            }
        };

        template<typename T_Table>
        struct Line_size {
            static constexpr size_t value() {
                return Line<sizeof(T_Table)>::size();
            }
        };


        template<size_t LINE_SIZE>
        auto buffer(const Line<LINE_SIZE> &);

        template<typename T_Table>
        auto to_line(T_Table &&table) -> Line<sizeof(T_Table)>;

        template<size_t LINE_SIZE>
        auto buffer(const Line<LINE_SIZE> &l) {
            Buffer<Line<LINE_SIZE>::size()> buf{};
            buf.put(l.id);
            buf.put(l.checksum);
            auto state = static_cast<unsigned char>(l.state);
            buf.put(state);
            buf.put(l.data);
            return buf;
        }


        template<typename T_Table>
        auto to_line(T_Table &&table) -> Line<sizeof(T_Table)> {
            db::assert_is_table<T_Table>();
//            using std::move;
//            Line<512> line{move(table)};
            std::cout << "sizeof:" << sizeof(T_Table) << "\n";
//            sizeof(std::remove_const<std::remove_reference<std::decay<decltype(table)>::type>::type>::type) << "\n";
            Line<sizeof(table)> line{std::move(table)};
            return line;
        }
//        template<typename T, size_t s = 0>
//        struct Line_size {
//            typedef T type;
//
//            static constexpr size_t value() {
//                return 0;
//            }
//        };
//
//        template<>
//        struct Line_size<Line> {
//            typedef Line type;
//
//            static constexpr size_t value() {
//                return sizeof(db::rid) + sizeof(db::crc32) + sizeof(db::State) + (sizeof(db::raw) * type::bytes());
//            }
//        };


    }
}

#endif //FS_LINE_H

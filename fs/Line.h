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
        template<size_t T_bytes, typename T_hash_type>
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
                return multipleOf(
                        sizeof(db::rid) + sizeof(T_hash_type) + sizeof(db::RState) + sizeof(db::raw<T_bytes>));
            }
        };

        template<typename T_Meta>
        struct Line_size {
        private:
            using T_Table = typename T_Meta::Table;
            using hash_algh = typename T_Meta::hash_algh;
        public:
            Line_size() {
//                db::assert_is_meta<T_Meta>();
            }

            static constexpr size_t value() {
                return Line<sizeof(T_Table), hash_algh>::size();
            }
        };


        template<size_t LINE_SIZE, typename hash_algh>
        auto buffer(const Line<LINE_SIZE, hash_algh> &);

        template<typename T_Table, typename hash_algh>
        auto to_line(T_Table &&table) -> Line<sizeof(T_Table), hash_algh>;

        template<size_t LINE_SIZE, typename hash_algh>
        auto buffer(const Line<LINE_SIZE, hash_algh> &l) {
            Buffer<Line<LINE_SIZE, hash_algh>::size()> buf{};
            buf.put(l.id);
            buf.put(l.checksum);
            auto state = static_cast<unsigned char>(l.state);
            buf.put(state);
            buf.put(l.data);
            return buf;
        }


        template<typename T_Table, typename hash_algh>
        auto to_line(T_Table &&table) -> Line<sizeof(T_Table), hash_algh> {
            db::assert_is_table<T_Table>();
//            using std::move;
//            Line<512> line{move(table)};
            std::cout << "sizeof:" << sizeof(T_Table) << "\n";
//            sizeof(std::remove_const<std::remove_reference<std::decay<decltype(table)>::type>::type>::type) << "\n";
            Line<sizeof(table), hash_algh> line{std::move(table)};
            return line;
        }

    }
}

#endif //FS_LINE_H

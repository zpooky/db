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
#include <iostream>

namespace db {
    namespace fs {
        template<size_t BYTES>
        struct Line {
            db::rid id;
            db::crc32 checksum;
            db::State state;
            db::raw<BYTES> data;

            Line(Table &&table) {
                id = 1;

            }

            static constexpr size_t bytes() {
                return BYTES;
            }

            static constexpr size_t size() {
                return sizeof(db::rid) + sizeof(db::crc32) + sizeof(db::State) + sizeof(db::raw<BYTES>);
            }
        };

        template<typename T_Table>
        auto to_line(T_Table &&table) -> Line<sizeof(T_Table)>;

        template<size_t LINE_SIZE>
        auto buffer(const Line<LINE_SIZE> &);

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

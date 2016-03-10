//
// Created by spooky on 2016-03-03.
//

#ifndef DB_RAW_H
#define DB_RAW_H

#include <stddef.h>
#include "../shared/fs.h"
#include <type_traits>
#include <utility>

namespace db {
    namespace fs {
        template<size_t BYTES>
        struct Line {
            db::rid id;
            db::crc32 checksum;
            db::State state;
            db::raw data[BYTES];

            Line(Table &&table) {
                id = 1;

            }

            static constexpr size_t bytes() {
                return BYTES;
            }

            static constexpr size_t size() {
                return sizeof(db::rid) + sizeof(db::crc32) + sizeof(db::State) + (sizeof(db::raw) * BYTES);
            }
        };

        auto line(db::Table &&table) {
            using std::move;
            Line<sizeof(table)> line{move(table)};
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

#endif //DB_RAW_H

//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_FORMAT_H
#define PROJECT_FORMAT_H

#include "../../../shared/vfs.h"
#include "../SegmentFile.h"
/*
 * Header:
 * size in bytes:
 * |unsigned short|db::table::name::length|unsigned short|unsigned short|
 *
 * header fields:
 * .-------------------------------------------------------------------------------.
 * |db segment format version|Table name|Line size|Table version|
 * `-------------------------------------------------------------------------------`
 *---
 *partition type|partition meta data length|meta data|
 * partition type:
 * none                     : 0
 * day based partitioning   : 1
 *
 * endianess - ??
 */
namespace db {
    namespace segment {

        using db::fs::SegmentFile;
        using db::fs::Line_size;
        using db::fs::FileWriter;
        template<typename T_Meta>
        class Format {
        private:
            using T_Table = typename T_Meta::Table;
            const Directory m_root;
        public:
            using latest = Format<T_Meta>;

            explicit Format(const Directory &root) :
                    m_root{root} {
                db::assert_is_table<T_Table>();
            }

            SegmentFile create(const Filename &);

        private:
        };

        template<typename T_Meta>
        SegmentFile Format<T_Meta>::create(const Filename &filename) {
            using capacity = unsigned long long;
            const size_t line_size = Line_size<T_Meta>::value();
            const size_t lines = T_Meta::lines();
            capacity target = line_size * lines;
            //
            File file = m_root.cd(filename);
            FileWriter stream{file};
            //
            std::array<char, db::vfs::page::default_size> buf{0};
            do {
                size_t counter = std::min<size_t>(buf.size(), target);
                target = target - counter;
                stream.init_write(buf, counter);
            } while (target > 0);
            stream.flush();
            return SegmentFile{file, line_size, lines};
        }
    }
}

#endif //PROJECT_FORMAT_H

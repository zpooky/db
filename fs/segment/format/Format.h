//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_FORMAT_H
#define PROJECT_FORMAT_H

#include "../../../shared/vfs.h"
#include "../../../shared/conversions.h"
#include "../SegmentFile.h"
#include "../../FileReader.h"
#include "../PresentSet.h"
#include "../../Line.h"
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
        using db::PresentSet;


        template<typename T_Meta>
        class V1SegmentInit {
        private:
            using T_Table = typename T_Meta::Table;
            const Directory m_root;
        public:
            explicit V1SegmentInit(const Directory &root) :
                    m_root{root} {
                db::assert_is_table<T_Table>();
            }

            SegmentFile create(const Filename &);
        };


        template<typename T_Meta>
        class V1SegmentParser {
        private:
            using T_Table = typename T_Meta::Table;
        public:
            V1SegmentParser() {
                db::assert_is_table<T_Table>();
            }

            PresentSet<T_Meta> parse(const File &);

            db::segment::id get_id(const File &);
        };

        template<typename T_Meta>
        SegmentFile V1SegmentInit<T_Meta>::create(const Filename &filename) {
            using capacity = unsigned long long;
            constexpr size_t line_size = Line_size<T_Meta>::value();
            constexpr size_t lines = T_Meta::lines();
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

        template<typename T_Meta>
        PresentSet<T_Meta> V1SegmentParser<T_Meta>::parse(const File &file) {
            using db::fs::FileReader;
            using db::Buffer;
            using db::fs::Line;
            using db::fs::Table_size;
            using Line_t = Line<Table_size<T_Table>::value(), typename T_Meta::hash_algh>;
            //
            const size_t line_size = Line_size<T_Meta>::value();
            constexpr size_t lines = T_Meta::lines();
            //
            std::bitset<lines> res;
            FileReader fr(file);
            //TODO advice to skip page cache
            size_t current = 0;
            while (current++ < lines) {
                Buffer<line_size> buffer;
                fr.read(buffer);
                buffer.flip();
                Line_t line{buffer};
                if (line.id == db::EMPTY_LINE) {
                    res[current] = false;
                } else {
                    res[current] = true;
                }
            }
            return PresentSet<T_Meta>{res};
        }

        template<typename T_Meta>
        db::segment::id V1SegmentParser<T_Meta>::get_id(const File &file) {
            auto fname = file.filename();
            return db::to_uint64(fname.name);
        }

        class Format {
        private:
        public:
            template<typename T_Meta>
            using latest = V1SegmentInit<T_Meta>;

            template<typename T_Meta>
            static constexpr V1SegmentParser<T_Meta> parser(db::segment::version v) {
                //TODO version support
                return V1SegmentParser<T_Meta>{};
            }

        };
    }
}

#endif //PROJECT_FORMAT_H

//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../../shared/entities.h"
#include "../../shared/Assertions.h"
#include "../FileWriter.h"
#include "SegmentJournal.h"

/*
 * Header:
 * .----------------------.
 * |Table name | Line size|
 * `---------------------
 */

namespace db {
    namespace fs {
        class SegmentFile {
        private:
            const FileWriter m_file;
            const size_t m_line_size;
            const size_t m_number;
        public:

            SegmentFile(const File &file, size_t line_size, size_t number) : m_file{file},
                                                                             m_line_size{line_size},
                                                                             m_number{number} {
            }
        };

        namespace impl {
            class SegmentFileInit {
            private:
                const File m_file;
            public:
                SegmentFile create(size_t line_size, size_t number) const;

                const File &get_file() const {
                    return m_file;
                }
            };

            template<typename T_Table>
            class SegmentFileInitJournal {
            private:
                SegmentFileInit &m_init;
                SegmentJournal<T_Table> &m_journal;
            public:
                SegmentFileInitJournal(SegmentFileInit &init, SegmentJournal<T_Table> &journal) : m_init{init},
                                                                                                  m_journal{journal} {
                    db::assert_is_table<T_Table>();
                }

                SegmentFile create(size_t number);
            };

            template<typename T_Table>
            SegmentFile SegmentFileInitJournal<T_Table>::create(size_t number) {
                auto id = m_journal.start(m_init.get_file());
                SegmentFile result = m_init.create(Line_size<T_Table>::value(), number);
                m_journal.prepare(id);
                m_journal.commit(id);
                return result;
            }
        }

        class PresentSet {
        private:
        };

        template<typename t_Table>
        class Segment {
        private:
            SegmentFile m_file;
            //State
        public:
            Segment(const SegmentFile &file) : m_file{file} {
                db::assert_is_table<t_Table>();
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE> &line) {
                return true;
            }
        };


    }
}


#endif //FS_SEGMENT_H

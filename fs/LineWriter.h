//
// Created by spooky on 2016-04-09.
//

#ifndef PROJECT_LINEWRITER_H
#define PROJECT_LINEWRITER_H

#include "../shared/entities.h"
#include "FileWriter.h"

namespace db {
    namespace fs {
        class LineWriter {
        private:
            FileWriter m_fileWriter;
        public:
            LineWriter(const File &file) : m_fileWriter{file} {
            }

            template<size_t size>
            void write(const Reservation &r, const Line<size> &l) {
                auto buf = buffer(l);
                m_fileWriter.write(buf, r.position);
            }
        };

    }
}


#endif //PROJECT_LINEWRITER_H

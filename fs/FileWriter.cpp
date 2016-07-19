//
// Created by spooky on 2016-03-07.
//

#include "FileWriter.h"

namespace db {

        void FileWriter::error(const char *func, int ret) const {
            if (ret == -1) {
                int errnum = errno;
                string err(func);
                err.append("(");
                err.append(m_file.path);
                err.append("): ");
                err.append(strerror(errnum));
                throw std::runtime_error(err);
            }
        }
}

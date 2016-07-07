//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_FILEREADER_H
#define PROJECT_FILEREADER_H

#include "../shared/entities.h"
#include <unistd.h>
#include <fcntl.h>
#include "../shared/Buffer.h"

namespace db {
    namespace fs {
        class FileReader {
        private:
            File m_file;
            int m_fd;
        public:
            explicit FileReader(const File &file) :
                    m_file(file),
                    m_fd{open(file.path.c_str(), O_RDONLY)} {
//                error("FileReader", m_fd);
            }

            FileReader(FileReader &&o) :
                    m_file(o.m_file),
                    m_fd(o.m_fd) {

                o.m_fd = -1;
            }

            FileReader(const FileReader &o) = delete;

            ~FileReader() {
                if (m_fd != -1) {
                    ::close(m_fd);
                    m_fd = -1;
                }
            }

            template <size_t capacity>
            int read(db::fs::Buffer<capacity>& buffer);
        };
        template <size_t capacity>
        int FileReader::read(db::fs::Buffer<capacity>& buffer){
            return 0;
        }
    }
}


#endif //PROJECT_FILEREADER_H

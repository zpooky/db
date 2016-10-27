//
// Created by spooky on 2016-10-06.
//

#ifndef PROJECT_PAGEBUFFEREDFILEWRITER_H
#define PROJECT_PAGEBUFFEREDFILEWRITER_H

#include <vector>
#include "JournalFile.h"

/**
 * This buffers writes in a page sized and aligned buffer.
 * To avoid the reread of page frames, the buffer is written all ways at once
 * meaning there is no point of reading the page before replacing it fully.
 * The same data is written to the same page if a force full flush is triggered.
 * Tnly when a page buffer is full create a new buffer and write to the next page frame.
 * Uninitialized data is represented as zeroes.
 *
 * A write will trigger a buffer write and flush:
 * - if the page buffer is full
 * - if a flush msg is received
 * If a write occurs where none of thease two conditions are met no fs write is performed the data is only buffered
 * either until the next write and the conditions are met or when the force_flash function is called.
 */
namespace db {
    class JournalPageBufferedFileWriter {
    private:
       const JournalFile& m_file;
    public:
        explicit JournalPageBufferedFileWriter(const JournalFile &file) :
                m_file(file){

        }

        JournalPageBufferedFileWriter(const JournalPageBufferedFileWriter &&) = delete;

        JournalPageBufferedFileWriter(const JournalPageBufferedFileWriter &) = delete;

        JournalPageBufferedFileWriter &operator=(const JournalPageBufferedFileWriter &&) = delete;

        JournalPageBufferedFileWriter &operator=(const JournalPageBufferedFileWriter &) = delete;
        ~JournalPageBufferedFileWriter(){
        }

        /* TODO int is placeholder
         * return the id of the forced flush events?
         */
        std::vector<int> write(const std::vector<int> &) {
          return {};
        }

        void force_flush() {
        }
    };
}

#endif //PROJECT_PAGEBUFFEREDFILEWRITER_H

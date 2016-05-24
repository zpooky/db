//
// Created by spooky on 2016-03-28.
//

#ifndef PROJECT_VFS_H
#define PROJECT_VFS_H

#include <stddef.h>
#include "../shared/entities.h"

//#ifndef BLKGETSIZE
//#define BLKGETSIZE _IO(0x12,96)                   /* return device size */
//#endif
//
//#ifndef BLKPBSZGET
//#define BLKSSZGET  _IO(0x12,123)/* get block device sector size */
//#endif

namespace db {
    namespace vfs {
        /**
         * the harddrive sector size is the minimum amount of data the hdd can read/write in a single go.
         */
        namespace sector {
            /**
             * bytes
             */
            static const size_t default_size = 512;

            size_t size(const char *s);
        }
        /**
         * the page size is the minimum unit of data the OS kernel can read/write
         */
        namespace page {
            /**
             * bytes
             */
            static const size_t default_size = 4 * 1024;

            size_t size();
        }

        //
        db::Directory mkdir(const Directory &d);
    }
}

#endif //PROJECT_VFS_H

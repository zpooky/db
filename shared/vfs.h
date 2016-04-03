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
        static const size_t default_sector_size = 512;
        size_t sector_size(const char *s);
        //
        void mkdir(const Directory &d);
    }
}

#endif //PROJECT_VFS_H

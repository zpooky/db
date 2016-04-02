//
// Created by spooky on 2016-03-28.
//

#ifndef PROJECT_VFS_H
#define PROJECT_VFS_H


#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>/*close*/

//#ifndef BLKGETSIZE
//#define BLKGETSIZE _IO(0x12,96)                   /* return device size */
//#endif
//
//#ifndef BLKPBSZGET
//#define BLKSSZGET  _IO(0x12,123)/* get block device sector size */
//#endif


class VirtualFileSystem {
public:
    size_t sector_size(const char *s) {

        int fd = open("/dev/sda", O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            return 512l;
        }
//        printf("fd: %d\n", fd);

        int size = 0;
        if (ioctl(fd, BLKGETSIZE, &size) != 0) {
//            size = 0;
        }

        int logicalsectsize = 0;
        int retval = ioctl(fd, BLKSSZGET, &logicalsectsize);
        if (retval < 0) {
//            printf("Can't get logical sector size. Return code: %d\n", retval);
//            logicalsectsize = 0;
            return 512l;
        }
        close(fd);
//        printf("%d %d\n", size, logicalsectsize);
        return logicalsectsize;
    }
};


#endif //PROJECT_VFS_H

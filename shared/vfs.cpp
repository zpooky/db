//
// Created by spooky on 2016-03-28.
//

#include "vfs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>/*close*/
#include <stdexcept>


size_t db::vfs::sector_size(const char *s) {

    int fd = open("/dev/sda", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return default_sector_size;
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
        return default_sector_size;
    }
    close(fd);
//        printf("%d %d\n", size, logicalsectsize);
    return logicalsectsize;
}


void db::vfs::mkdir(const Directory &d) {

}
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
#include <unistd.h>/*close,..*/
#include <stdexcept>


size_t db::vfs::sector::size(const char *s) {

    int fd = ::open("/dev/sda", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return default_size;
    }
//        printf("fd: %d\n", fd);

    int size = 0;
    if (::ioctl(fd, BLKGETSIZE, &size) != 0) {
//            size = 0;
    }

    int logicalsectsize = 0;
    int retval = ::ioctl(fd, BLKSSZGET, &logicalsectsize);
    ::close(fd);
    if (retval < 0) {
//            printf("Can't get logical sector size. Return code: %d\n", retval);
//            logicalsectsize = 0;
        return default_size;
    }
//        printf("%d %d\n", size, logicalsectsize);
    return logicalsectsize;
}

size_t db::vfs::page::size() {
    int result = ::getpagesize();
    if (result < 0) {
        return default_size;
    }
    return result;
}


void db::vfs::mkdir(const db::Directory &d) {
    const auto &path = d.path;
    printf("mkdir(%s) ", path.c_str());
    int status = ::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0) {
        printf("ret code: %d", status);
    }
    printf("\n");
}
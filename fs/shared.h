#ifndef _FS_SHARED_H
#define _FS_SHARED_H

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>

void error(const char *fun, int ret);

void error(const char *func, int ret) {
  if (ret == -1) {
    int errnum = errno;
    std::string err(func);
    err.append("(");
    // err.append(m_file.path);
    err.append("): ");
    err.append(::strerror(errnum));
    throw std::runtime_error(err);
  }
}
void error_mmap(const char *func, const void *ptr) {
  if (ptr == MAP_FAILED) {
    std::string err(func);
    err.append("(");
    // err.append(m_file.path);
    err.append("): ");
    err.append("MMAP_FAILED");
    throw std::runtime_error(err);
  }
}
#endif

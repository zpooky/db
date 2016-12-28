#ifndef _FS_SHARED_H
#define _FS_SHARED_H

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>

void error(const char *fun, int ret);

void throw_error(const char *);

void error(const char *func, int ret) {
  if (ret == -1) {
    throw_error(func);
  }
}
void error_mmap(const char *func, const void *ptr) {
  if (ptr == MAP_FAILED) {
    throw_error(func);
  }
}
void throw_error(const char *func){
    int errnum = errno;
    std::string err(func);
    err.append("(");
    // err.append(m_file.path);
    err.append("): ");
    err.append(::strerror(errnum));
    throw std::runtime_error(err);
}
#endif

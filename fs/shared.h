#ifndef _FS_SHARED_H
#define _FS_SHARED_H

#include <cstring>
#include <string>
#include <stdexcept>

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
#endif

//
// Created by spooky on 2016-07-19.
//

#ifndef PROJECT_CONVERSIONS_H
#define PROJECT_CONVERSIONS_H

#include <cstdint>
#include <string>

namespace db {

    std::uint64_t to_uint64(const char *);

    std::uint64_t to_uint64(const std::string &);
}

#endif //PROJECT_CONVERSIONS_H

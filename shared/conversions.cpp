//
// Created by spooky on 2016-07-19.
//

#include "conversions.h"
#include <cstdlib>
#include <string>

namespace db {

    std::uint64_t to_uint64(const char *str) {
        return std::atol(str);
    }

    std::uint64_t to_uint64(const std::string &str) {
        return to_uint64(str.c_str());
    }
}

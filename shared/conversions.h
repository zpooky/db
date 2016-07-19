//
// Created by spooky on 2016-07-19.
//

#ifndef PROJECT_CONVERSIONS_H
#define PROJECT_CONVERSIONS_H

#include <cstdint>
#include <string>

namespace db {
    template<typename T>
    T to(const char *);

    template<typename T>
    T to(const std::string &);


    template<>
    uint64_t to(const char *str) {
        return std::atol(str);
    }

    template<>
    uint64_t to(const std::string &str) {
        return to<uint64_t>(str.c_str());
    }

    template<typename T>
    T to(const char *) = delete;

    template<typename T>
    T to(const std::string &) = delete;

}

#endif //PROJECT_CONVERSIONS_H

//
// Created by spooky on 2016-04-03.
//

#include "entities.h"


namespace db {
    Directory Directory::cd(string &&d) const {
        string copy = path;
        if (copy.at(copy.length() - 1) == '/') {
            copy.append("/");
        }
        copy.append(d);
        return Directory{std::move(copy)};
    }
}
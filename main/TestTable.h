//
// Created by spooky on 2016-03-07.
//

#ifndef PROJECT_TESTTABLE_H
#define PROJECT_TESTTABLE_H

#include "../shared/fs.h"

struct TestTable : public db::Table {
    unsigned long id;

    size_t size() const noexcept {
        return sizeof(id);
    }

};

#endif //PROJECT_TESTTABLE_H

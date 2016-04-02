//
// Created by spooky on 2016-03-07.
//

#ifndef PROJECT_TESTTABLE_H
#define PROJECT_TESTTABLE_H

#include "../shared/fs.h"

struct TestTable : public db::Table {
    char data[5];
    unsigned long id;

    TestTable() : id{1}, data{"hell"} {

    }

//    size_t size() const noexcept {
//        return sizeof(TestTable);
//    }

};

#endif //PROJECT_TESTTABLE_H

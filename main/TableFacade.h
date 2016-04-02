//
// Created by spooky on 2016-04-02.
//

#ifndef PROJECT_TABLEFACADE_H
#define PROJECT_TABLEFACADE_H

#include "../shared/Assertions.h"

namespace db {

    template<typename T_Table>
    class TableFacade {
    private:
    public:
        TableFacade() {
            assert_is_table<T_Table>();
        }
    };
}

#include "TableFacade.cpp"

#endif //PROJECT_TABLEFACADE_H

//
// Created by spooky on 2016-04-02.
//

#ifndef PROJECT_TABLEFACADE_H
#define PROJECT_TABLEFACADE_H

#include "../shared/Assertions.h"
#include "../shared/fs.h"


namespace db {

    template<typename T_Table>
    class TableFacade {
    private:
        const Directory m_root;
    public:
        TableFacade(Directory &&root) : m_root{root} {
            assert_is_table<T_Table>();
            auto table_root = root.cd(T_Table::table_name());
            db::vfs::mkdir(table_root);
        }

        Reservation reserve();


    };
}

#include "TableFacade.cpp"

#endif //PROJECT_TABLEFACADE_H

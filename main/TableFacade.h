//
// Created by spooky on 2016-04-02.
//

#ifndef PROJECT_TABLEFACADE_H
#define PROJECT_TABLEFACADE_H

#include "../shared/Assertions.h"
#include "../shared/fs.h"
#include "../fs/segment/Reservations.h"
#include "../fs/segment/Segments.h"
#include "../shared/vfs.h"


namespace db {

    template<typename t_Table>
    class TableFacade {
    private:
        const Directory m_root;
        Reservations<t_Table> m_reservations;
        Segments<t_Table> m_segments;
    public:
        TableFacade(Directory &&root) : m_root{root} {
            assert_is_table<t_Table>();
            auto table_root = root.cd(t_Table::table_name());
            db::vfs::mkdir(table_root);
            m_reservations = Reservations<t_Table>{table_root};
        }

        Reservations<t_Table> &get_reservations();

        Segments<t_Table> &get_segments();
    };

    template<typename t_Table>
    Reservations<t_Table> &TableFacade<t_Table>::get_reservations() {
        return m_reservations;
    }
    template<typename t_Table>
    Segments<t_Table> &TableFacade<t_Table>::get_segments() {
        return m_segments;
    }

}


#endif //PROJECT_TABLEFACADE_H

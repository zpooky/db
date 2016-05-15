//
// Created by spooky on 2016-05-12.
//

#ifndef _SP_LIST_H
#define _SP_LIST_H

namespace sp {
    template <typename T>
    class List {
    public:
        template <typename Predicate, typename Supplier>
        T& find(Predicate p, Supplier s) {
        }
    };
}

#endif

//
// Created by spooky on 2016-03-07.
//

#ifndef PROJECT_TESTTABLE_H
#define PROJECT_TESTTABLE_H

#include "../shared/fs.h"
#include <array>

struct TestTable : public db::Table {
    const char data[5];
    const unsigned long id;

    TestTable() :
            data{"hell"},
            id{1} {

    }

private:
    using name_type = db::table::name::type;

//    template<class T, std::size_t N, std::size_t... I>
//    constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&a)[N], std::index_sequence<I...>) {
//        return {{a[I]...}};
//    }
//
//    template<class T, std::size_t N>
//    constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N]) {
//        return to_array_impl(a, std::make_index_sequence<N>{});
//    }

public:
    constexpr static name_type table_name() {
//        return to_array("test_table");
        return {'t', 'e', 's', 't', '_', 't', 'a', 'b', 'l', 'e', 0};
    }

//    size_t size() const noexcept {
//        return sizeof(TestTable);
//    }

};

#endif //PROJECT_TESTTABLE_H

//
// Created by spooky on 2016-03-03.
//

#ifndef DB_JOURNAL_H
#define DB_JOURNAL_H

#include "../shared/Assertions.h"
#include "../shared/fs.h"

namespace journal {
// class Journal {
// private:
//   Journal(const Journal &) = delete;
//
// public:
//   Journal() {
//   }
//
//   journal::id begin();
//
//   template <typename t_Table>
//   void prepare(journal::id, const db::Reservation<t_Table> &);
//
//   void commit(journal::id);
// };
//
// Journal &instance();
//
// Journal &instance() {
//   static Journal inst{};
//   return inst;
// }
//
// journal::id Journal::begin() {
//   return 1l;
// }
//
// template <typename t_Table>
// void Journal::prepare(journal::id id, const db::Reservation<t_Table> &) {
// }
//
// void Journal::commit(journal::id) {
// }
}

#endif // DB_JOURNAL_H

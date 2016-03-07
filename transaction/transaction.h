//
// Created by spooky on 2016-03-06.
//

#ifndef PROJECT_TRANSACTION_H
#define PROJECT_TRANSACTION_H

namespace db {
    namespace transaction {
        enum class State : unsigned char {
            PENDING, PREPARED, COMMIT, ROLLBACK
        };
    }
}
#endif //PROJECT_TRANSACTION_H

//
// Created by spooky on 2016-04-03.
//

#ifndef PROJECT_RESERVATIONS_H
#define PROJECT_RESERVATIONS_H

#include "../../shared/entities.h"
#include "../../shared/fs.h"
#include <utility>
#include <bitset>

namespace db {

    template <size_t t_Size>
    class ReservationSet {
    private:
        std::bitset<t_Size> m_b;
    public:
    };

    template<typename t_Table>
    class Reservations {
    private:
        const Directory m_root;
    public:

        Reservations(const Directory &root) : m_root{root} {
        }

        Reservations(const Reservations &o) = delete;

        Reservations &operator=(Reservations<t_Table> &&other) {
            if (this != &other) {
                std::swap(m_root, other.m_root);
            }
            return *this;
        }

        Reservation reserve();
    };

    template<typename t_Table>
    Reservation Reservations<t_Table>::reserve() {
        return {1l};
    }

}

#endif //PROJECT_RESERVATIONS_H

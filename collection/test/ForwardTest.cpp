//
// Created by spooky on 2016-10-02.
//


#include "gtest/gtest.h"
#include <iostream>

using namespace std;

struct NoCopyAndMove {
    const int m_data;

    int data() {
        return m_data;
    }

    explicit NoCopyAndMove(uint32_t p_data = 0) : m_data(p_data) {
    }

    ~NoCopyAndMove() {

    }

    NoCopyAndMove(NoCopyAndMove &&) = delete;

    NoCopyAndMove(const NoCopyAndMove &) = delete;

    NoCopyAndMove &operator=(NoCopyAndMove &&) = delete;

    NoCopyAndMove &operator=(const NoCopyAndMove &) = delete;

};

template<typename T>
void f1(T &&n) {
    cout << "|" << n.data() << "\n";
}

template<typename T>
void f(T &&n) {
    f1(std::forward<T>(n));
}

template<typename T>
void f_move(T &&n) {
    f1(std::move(n));
}

template<typename T>
struct Logic {

    void f1(T &&n) {
        cout <<"<"<< n.data() << "\n";
    }

    void f(T &&n) {
        f1(std::forward<T>(n));
    }

    void f_move(T &&n) {
        f1(std::move(n));
    }
};

TEST(ForwardTree, test) {
    f(NoCopyAndMove(1));
    f_move(NoCopyAndMove(2));
    Logic<NoCopyAndMove> l;
    l.f_move(NoCopyAndMove(3));
    l.f(NoCopyAndMove(4));
}

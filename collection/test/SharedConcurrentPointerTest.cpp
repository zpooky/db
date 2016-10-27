//
// Created by spooky on 2016-10-04.
//

#include "gtest/gtest.h"
#include "ConcurrentPointer.h"

struct Entry {
    void prin() {
    }
};

struct NoStuff {
    NoStuff() {
    }

    NoStuff(const NoStuff &&) = delete;

    NoStuff(const NoStuff &) = delete;

    NoStuff &operator=(const NoStuff &) = delete;

    NoStuff &operator=(const NoStuff &&) = delete;

    ~NoStuff() {
        std::cout << "NoStuff destruct\n";
    }

};

struct DefaultStuff {
    NoStuff n;

    DefaultStuff() {
    }

    DefaultStuff(DefaultStuff &&) = default;

    DefaultStuff(const DefaultStuff &) = default;

    DefaultStuff &operator=(const DefaultStuff &) = default;

    DefaultStuff &operator=(DefaultStuff &&) = default;

//    ~DefaultStuff() {
//    }
};

TEST(SharedConcurrentPointerTest, test) {
    using ptr_t = sp::ConcurrentPointer<Entry>;
//    static_assert(std::is_trivially_copyable<NoStuff>::value, "NoStuff not");
//    static_assert(std::is_trivially_copyable<DefaultStuff>::value, "DefaultStuff not");
//    static_assert(std::is_trivially_copyable<Entry>::value, "e not");
    static_assert(std::is_trivially_copyable<Entry *>::value, "e* not");
//    static_assert(std::is_trivially_copyable<ptr_t>::value, "not");
    ptr_t s(new Entry);
    ptr_t s1(s);
    ptr_t s2(s1);
    ptr_t s3(s);
    ptr_t s4(std::move(s1));
    s.operator*().prin();
    (*s).prin();
//    s.prin();
    s.operator->()->prin();
    s->prin();
    auto entry = std::make_unique<Entry>();
    (*entry).prin();
    std::cout << std::endl << alignof(double) << std::endl;
    std::cout << std::endl << alignof(int) << std::endl;
    std::cout << std::endl << alignof(unsigned long long) << std::endl;
}

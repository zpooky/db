//
// Created by spooky on 2016-07-20.
//

#include "gtest/gtest.h"
#include "../LIFO.h"
#include <unordered_set>
#include <future>

class LIFOTest : public ::testing::TestWithParam<size_t> {

};

INSTANTIATE_TEST_CASE_P(Threads, LIFOTest, ::testing::Range(size_t(1), size_t(15)));

TEST_F (LIFOTest, test) {

    LIFO<size_t> l;
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(1lu);
    ASSERT_EQ(1ul, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(20lu);
    ASSERT_EQ(20ul, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(1lu);
    l.push_front(20lu);
    size_t ssss(30);
    l.push_front(ssss);

    ASSERT_EQ(1lu, l.pop(0ul));
    ASSERT_EQ(20lu, l.pop(0ul));
    ASSERT_EQ(30lu, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));
}

template<typename T>
auto join(T &futures) -> std::vector<decltype(futures.front().get())> {
    std::vector<decltype(futures.front().get())> res;
    for (auto &f : futures) {
        res.push_back(f.get());
    }
    return res;
}

TEST_P(LIFOTest, paralell_push) {
    const size_t max = 100000;
    const size_t threads = GetParam();
    const size_t items = max * threads;

    using Vec_t = std::unordered_set<size_t>;
    using Future_t = std::future<Vec_t>;


    LIFO<size_t> list;
    std::vector<Future_t> v;
//    std::atomic<size_t> cnt(0);
    for (size_t i = 0; i < threads; ++i) {
        /**
         * setup threds
         */
        std::packaged_task<Vec_t()> task([max, i, &list]() {
            size_t cnt = i * max;
            const size_t maxs = max + cnt;
            Vec_t result;
            while (cnt++ < maxs) {
                result.insert(cnt);
                list.push_front(cnt);
            }
            return result;
        });
        v.push_back(task.get_future());
        std::thread t(std::move(task));
        t.detach();
    }
    /**
     * collect data
     */
    join(v);

    std::unordered_set<size_t> drain(items);
    const size_t max_size_t = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < items; ++i) {
        size_t v = list.pop(max_size_t);
        ASSERT_NE(max_size_t, v);
        ASSERT_TRUE(drain.insert(v).second);
    }
    ASSERT_EQ(max_size_t, list.pop(max_size_t));

}
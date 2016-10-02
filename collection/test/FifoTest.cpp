//
// Created by spooky on 2016-07-20.
//

#include "gtest/gtest.h"
#include "../MpScFifo.h"
#include <unordered_set>
#include <future>
#include <Barrier.h>

class FifoTest : public ::testing::TestWithParam<size_t> {

};

INSTANTIATE_TEST_CASE_P(Threads, FifoTest, ::testing::Range(size_t(1), size_t(10)));

using namespace sp;

TEST_F (FifoTest, test) {

    MpScFifo<size_t> l;
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_back(1lu);
    ASSERT_EQ(1ul, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_back(size_t(20));
    ASSERT_EQ(size_t(20), l.pop(size_t(0)));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_back(1lu);
    l.push_back(20lu);
    size_t ssss(30);
    l.push_back(ssss);

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

std::vector<std::future<size_t>> unnamed(const size_t max,
                                         const size_t threads,
                                         MpScFifo<size_t> &list,
                                         Barrier &barrier) {
    std::atomic_thread_fence(std::memory_order::memory_order_release);
    std::vector<std::future<size_t>> v;
    for (size_t i = 0; i < threads; ++i) {
        /**
         * setup threads
         */
        std::packaged_task<size_t()> task([max, i, &barrier, &list]() {
            std::atomic_thread_fence(std::memory_order::memory_order_acquire);
            barrier.await();
            std::cout<<"start\n";
            size_t cnt = i * max;
            const size_t maxs = max + cnt;
            while (cnt < maxs) {
                list.push_back(cnt++);
            }
            return size_t(0);
        });
        v.push_back(task.get_future());
        std::thread t(std::move(task));
        t.detach();
    }
    return v;
}

TEST_P(FifoTest, paralell_push) {
    const size_t max = 100000;
    const size_t threads = GetParam();
    const size_t items = max * threads;

    using Future_t = std::future<size_t>;


    MpScFifo<size_t> list;
    sp::Barrier barrier(threads);

    std::vector<Future_t> v = unnamed(max, threads, list, barrier);
    /**
     * collect data
     */
    join(v);
    std::cout<<"join-"<<std::endl;

    std::unordered_set<size_t> drain(items);
    const size_t max_size_t = std::numeric_limits<size_t>::max();

    //Drain and test number of elements in Fifo
    for (size_t i = 0; i < items; ++i) {
        size_t v = list.pop(max_size_t);
        ASSERT_NE(max_size_t, v);
        ASSERT_TRUE(drain.insert(v).second);
    }
    ASSERT_EQ(max_size_t, list.pop(max_size_t));

    //Test that all elements are present in Fifo
    for (size_t i = 0; i < items; ++i) {
        ASSERT_NE(drain.end(), drain.find(i));
    }

}

TEST_P(FifoTest, paralell_push_concurrent_pop) {
    const size_t max = 100000;
    const size_t threads = GetParam();
    const size_t items = max * threads;

    using Future_t = std::future<size_t>;


    MpScFifo<size_t> list;
    sp::Barrier barrier(threads + 1);

    auto futures = unnamed(max, threads, list, barrier);

    size_t i=0;
    std::unordered_set<size_t> drain(items);
    const size_t max_size_t = std::numeric_limits<size_t>::max();

    barrier.await();
//    join(futures);

    while (i < items){
        size_t val = list.pop(max_size_t);
        if(val != max_size_t){
            ASSERT_TRUE(drain.insert(val).second);
            ++i;
        }
    }

    //Test that all elements are present in Fifo
    for (size_t i = 0; i < items; ++i) {
        ASSERT_NE(drain.end(), drain.find(i));
    }

}

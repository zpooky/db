//
// Created by spooky on 2016-07-08.
//

#include "../List.h"
#include "gtest/gtest.h"
#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <unordered_set>
#include <vector>

using sp::con::Stack;
using std::thread;
using std::vector;
using std::future;
using std::cout;
using std::endl;
using std::unordered_set;

template <typename T>
T &find(Stack<T> &l, const T find, T gen) {
  return l.find([&](const auto &l) { return find == l; }, [&] { return gen; });
}

TEST(List, foreach) {
  Stack<long> l;
  for (auto &i : l) {
    ASSERT_EQ(i, i);
  }
  l.push_front(1);
  for (auto &i : l) {
    ASSERT_EQ(i, i);
  }
  for (auto it = l.begin(); it != l.end(); ++it) {
    ASSERT_EQ(*it, *it);
  }
}

TEST(List, destruct_empty) {
  Stack<long> l;
}

TEST(List, push_front) {
  Stack<long> l;
  l.push_front(1l);
}

TEST(List, find_null_true) {
  Stack<long> l;
  long v = find(l, 1l, 1l);
  ASSERT_EQ(1l, v);
}

TEST(List, find_null_false) {
  Stack<long> l;
  long v = find(l, 3l, 1l);
  ASSERT_EQ(1l, v);
}

TEST(List, find_single_true) {
  Stack<long> l;
  l.push_front(2l);
  long v = find(l, 2l, 1l);
  ASSERT_EQ(2l, v);
}

TEST(List, find_single_false) {
  Stack<long> l;
  l.push_front(2l);
  long v = find(l, 3l, 1l);
  ASSERT_EQ(1l, v);
}

TEST(List, find_mutltiple) {
  Stack<size_t> l;
  size_t i = 0;
  while (i++ < 100) {
    {
      size_t v = find(l, i, i);
      ASSERT_EQ(v, i);
    }
    for (size_t check = 1; check < i; ++check) {
      size_t v = find(l, check, size_t(1000000));
      ASSERT_EQ(v, check);
    }
  }
}

TEST(List, push_front_find) {
  Stack<size_t> l;
  size_t i = 0;
  size_t is = 100;
  while (i++ < is) {
    l.push_front(std::move(i));
  }
  i = 0;
  while (i++ < is) {
    size_t v = find(l, i, is);
    ASSERT_EQ(v, i);
  }
}

template <typename T>
auto join(T &futures) -> vector<decltype(futures.front().get())> {
  vector<decltype(futures.front().get())> res;
  for (auto &f : futures) {
    res.push_back(f.get());
  }
  return res;
}

template <typename T>
void contains(const vector<unordered_set<T>> &vals, const T &find) {
  bool found = false;
  for (const auto &v : vals) {
    //        const auto it = ;
    if (v.find(find) != v.end()) {
      if (found) {
        ASSERT_EQ("", "duplicate in thread");
      }
      found = true;
    }
  }
  if (!found) {
    ASSERT_EQ(-1, find);
  }
}

TEST(List, threaded_find) {
  using Vec_t = unordered_set<size_t>;
  using Future_t = future<Vec_t>;

  Stack<size_t> list;
  vector<Future_t> v;
  std::atomic<size_t> cnt(0);
  for (size_t i = 0; i < 4; ++i) {
    /**
     * setup threds
     */
    std::packaged_task<Vec_t()> task([i, &cnt, &list]() {
      size_t i = 0;
      Vec_t result;
      while (i++ < 100000) {
        size_t current = cnt.fetch_add(1);
        result.insert(current);
        list.push_front(std::move(current));
      }
      return result;
    });
    v.push_back(task.get_future());
    thread t(std::move(task));
    t.detach();
  }
  /**
   * collect data
   */
  vector<Vec_t> thread_result = join(v);
  /**
   * test
   */
  //    for (const auto &c : list) {
  //        contains(thread_result, *c);
  //    }
}

#include <atomic>
#include <utility>
#include <functional>
#include <iostream>
#include <thread>
#include "Barrier.h"
#include <mutex>

/*
 * Compile with
 * g++ BinaryTree.cpp Barrier.cpp --std=c++14 -ggdb -lpthread && ./a.out
 * perf stat ./a.out
 */

class CountDownLatch {
  private:
    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::atomic<size_t> m_counter;
  public:
    explicit CountDownLatch(size_t size) :  m_counter{size}{
    }

    void countDown(){
      auto current = m_counter.load();
      retry:
      if(current == 0){
        return;
      }
      auto n = current - 1;
      if(!m_counter.compare_exchange_strong(current, n)){
        goto retry;
      }
     if(n == 0){
       std::unique_lock<std::mutex> lck(m_mutex);
       m_condition.notify_all();
     }
    }

    void await(){
      auto current = m_counter.load();
      if(current == 0){
        return;
      }
      std::unique_lock<std::mutex> lck(m_mutex);
      m_condition.wait(lck, [&] {
          return m_counter.load() == size_t(0);
      });
    }
};

template<size_t size>
class SameSizeAllocator {
  public:

};


template<typename Key, typename Type,typename Cmp = std::less<Key> >
class BinarTree {
  private:
    struct Node;
    using Node_t = std::atomic<Node*>;
  private:
    struct Iterator {
      private:
        Node* current;
        explicit Iterator(Node *c) : current(c){
        }
      public:
        Iterator() : Iterator(nullptr){
        }
    };
    struct Node {
      private:
      const Key m_key;
      Type m_value;
      Node_t m_less;
      Node_t m_greater;

      public:
      explicit Node(Key&& key,Type && value):m_key{std::move(key)}, m_value{std::move(value)},m_less{nullptr},m_greater{nullptr} {
      }

      ~Node(){
        auto less = m_less.load();
        if(less){
          delete less;
        }
        auto greater = m_greater.load();
        if(greater){
          delete greater;
        }
      }

      void insert(Node *o) {
        Cmp cmp;
        if(cmp(m_key, o->m_key)) {
          Node::next(o, m_less);
        }else {
          Node::next(o, m_greater);
        }
      }

      Iterator find(const Key&key) const {
        return nullptr;
      }

      static void next(Node *o,Node_t &next){
        Node* current = next.load();
        retry:
        if(current != nullptr){
           current->insert(o);
        }else 
        if(!next.compare_exchange_strong(current, o)) {
          goto retry;
        }
      }
    };

  Node_t m_root;
  public:
  BinarTree() : m_root{nullptr} {
  }

  ~BinarTree(){
    auto root = m_root.load();
    if(root){
      delete root;
    }
  }
  
  Node* insert(Key&& key,Type && value){
    auto node = new Node(std::move(key),std::move(value));
    Node::next(node,m_root);
    return node;
  }
};


struct Test {
  const size_t value;
  Test(size_t v) : value{v} {
  }
};

bool operator<(const Test&o,const Test&o1){
  return o.value<o1.value;
}

class alignas(16) float4 {
    float f[4];
};

int main(){
  std::cout<<sizeof(float4)<<"|"<<alignof(float4)<<"|"<<alignof(float)<<"\n";
  BinarTree<Test,Test> tree;
  const size_t THREADS(2);
  CountDownLatch b(THREADS);
  for(size_t ti(0);ti<THREADS;++ti){
    auto id = ti;
    std::thread t{[&,id] {
      const size_t IT = 10000;
      const size_t INIT = ti *IT;
      for(size_t i(INIT);i<INIT+IT;++i){
        tree.insert(size_t(i),size_t(i));
      }
      std::cout<<"done:"<<id<<"\n";
      b.countDown();
    }};
    t.detach();
  }
  b.await();
}

// #include <iostream>

#include "Bitset.h"
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <new>
#include <thread>
#include <tuple>
#include <unistd.h>

namespace db {
namespace con {
template <size_t slabs, size_t chunk_size, size_t chunks = 10>
class SlabAllocator {
private:
  struct Slab {
    static constexpr size_t size = chunks * chunk_size;
    sp::Bitset<size> reserved;
    uint8_t *mem;
    // bool b;

  public:
    static_assert(size % 8 == 0, "");
    Slab() : reserved(false), mem(nullptr) {
    }

    ~Slab() {
      if (mem) {
        delete[] mem;
        mem = nullptr;
      }
    }
    Slab(const Slab &) = delete;
    Slab(const Slab &&) = delete;

    void memory(uint8_t *m) {
      mem = m;
    }

    void *allocate() {
      auto off = reserved.swap_first(true);
      if (off != reserved.npos) {
        return mem + off;
      }
      return nullptr;
    }

    static constexpr size_t size_of() {
      return sizeof(reserved) + size + sizeof(mem);
    }

    size_t actual_size_of() const {
      return sizeof(reserved) + (mem != nullptr ? size : 0) + sizeof(mem);
    }
  };

  std::array<Slab, slabs> m_slabs;

public:
  SlabAllocator() : m_slabs{} {
    using namespace std::chrono_literals;
    constexpr size_t total(slabs * Slab::size);
    static_assert(total % 8 == 0, "");
    size_t attempt(total);
    size_t slab_idx(0);
    std::array<std::tuple<uint8_t *, bool>, slabs> tmp{};
    do {
      uint8_t *mem = new (std::nothrow) uint8_t[attempt];
      if (mem != nullptr) {
        for (size_t alc_strt(0); alc_strt < attempt; alc_strt += Slab::size) {
          bool gc(alc_strt == 0);
          tmp[slab_idx++] = std::make_tuple(mem + alc_strt, gc);
        }
        size_t remaining(total - slab_idx * Slab::size);
        attempt = std::min(attempt, remaining);
      } else {
        attempt = attempt / 2;
      }
      std::this_thread::sleep_for(2s);
      std::cout << slab_idx << "<" << slabs << "\n";
    } while (slab_idx < slabs);
    // std::sort(tmp);
    // m_slabs[slab_idx++].memory(mem + alc_strt);
    std::atomic_thread_fence(std::memory_order_release);
  }

  void *allocate() {
    // TODO pow2 mod optimisation
    const size_t start(getpid() % slabs);
    size_t index(start);
    do {
      auto &slab = m_slabs[index];
      void *alloc = slab.allocate();
      if (alloc != nullptr) {
        return alloc;
      }
      index = (index + 1) % slabs;
    } while (start != index);
    return nullptr;
  }

  void free(void *ptr) {
    // TODO binary search or something
    for (auto &slab : m_slabs) {
      if (slab.in_range(ptr)) {
        slab.free(ptr);
      }
    }
    assert(false);
  }
  static constexpr size_t size_of() {
    return Slab::size_of() * slabs;
  }
  size_t actual_size_of() const {
    size_t size(0);
    for (const auto &s : m_slabs) {
      size = size + s.actual_size_of();
    }
    return size;
  }
};
} // namespace con
} // namespace db

struct Line {
  uint64_t f1 = ~uint64_t(0);
  uint64_t f2 = 0;
  uint64_t f3 = ~uint64_t(0);
  uint64_t f4 = 0;
};
std::ostream &operator<<(std::ostream &o, const Line &l) {
  return o << l.f1 << "|" << l.f2 << "|" << l.f3 << "|" << l.f4 << "\n";
}
int main() {
  using Allocator = db::con::SlabAllocator<10, sizeof(Line)>;
  Allocator alc;
  std::cout << sizeof(Line) << "\n";
  std::cout << sizeof(Allocator) << "\n";
  void *mem = nullptr;
  size_t cnt(0);
  do {
    mem = alc.allocate();
    ++cnt;
    if (mem) {
      Line *l = new (mem) Line;
      std::cout << *l << "\n";
    }
    if (cnt >= 1024 * 1000) {
      return 1;
    }
  } while (mem != nullptr);
  std::cout << "cnt:" << cnt << "\n";
  std::cout << "sizeof:" << alc.size_of() << "B\n";
  std::cout << "actual sizeof:" << alc.actual_size_of() << "B\n";
  std::cout << "c++_sizeof:" << sizeof(alc) << "B\n";
}

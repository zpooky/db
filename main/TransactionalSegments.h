#ifndef _DB_TRANSACTIONAL_GUARD_H
#define _DB_TRANSACTIONAL_GUARD_H

#include "../segment/Segments.h"
#include "TransactionalGuard.h"

namespace tx {

template <typename Meta_t>
class TxSegments {
public:
  std::unique_ptr<db::Segments<Meta_t>> segments;

private:
  TxGuard guard;

public:
  TxSegments() : segments(nullptr), guard() {
  }

  TxSegments(db::Segments<Meta_t> *s, TxGuard &&g)
      : segments{s}, guard{std::move(g)} {
  }

  TxSegments(TxSegments<Meta_t> &&o) : segments(nullptr), guard() {
    swap(o);
  }

  TxSegments<Meta_t> &operator=(TxSegments<Meta_t> &&o) {
    if (this != &o) {
      assign(std::move(o));
    }
    return *this;
  }

  db::Segments<Meta_t> *operator->() const {
    return segments.get();
  }

  db::Segments<Meta_t> *operator->() {
    return segments.get();
  }

  ~TxSegments() {
  }

  void swap(TxSegments<Meta_t> &o) {
    std::swap(segments, o.segments);
    std::swap(guard, o.guard);
  }

private:
  void assign(TxSegments<Meta_t> o) {
    swap(o);
  }
};
}
#endif

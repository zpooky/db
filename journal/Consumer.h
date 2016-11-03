#ifndef _CONSUMER_H
#define _CONSUMER_H

namespace journal {
template <typename T>
struct Consumer {
  virtual void add(T &&) = 0;
};
}

#endif

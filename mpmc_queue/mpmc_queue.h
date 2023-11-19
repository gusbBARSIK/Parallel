#pragma once

#include <optional>
#include <cstdint>
#include <atomic>
#include <vector>
#include <thread>


template <class T>
class MPMCQueue {
 public:
  explicit MPMCQueue(uint64_t max_size)
    : items_( max_size )
      , capacity_( max_size )
      , head_( 0 )
      , tail_( 0 )
      {
        for(uint64_t i = 0; i < max_size ; ++i )
        {
          items_[i].version = i;
        }
      }

  bool Push(const T& value) {
    for (;;) {
        uint64_t head = head_.load();
        uint64_t idx = head & (capacity_ - 1);
        if (head == tail_.load() + capacity_) {
          return false;
        }
        if (items_[idx].version != head) {
            std::this_thread::yield();
            continue;
        } else {
            if (head_.compare_exchange_weak( head, head + 1 )) {
              items_[idx].value = value;
              ++items_[idx].version;
              return true;
            }
        }
    }
  }

  std::optional<T> Pop() {
    for (;;) {
        uint64_t tail = tail_.load( );
        uint64_t idx = tail & (capacity_ - 1);
        if (tail == head_.load()) {
          return std::nullopt;
        }
        if (items_[idx].version != (tail + 1)) {
            std::this_thread::yield();
            continue;
        } else {
            if (tail_.compare_exchange_weak( tail, tail + 1 )) {
              T value = items_[idx].value;
              items_[idx].version += capacity_ - 1;
              return value;
            }
        }
    }
  }

 private:
    struct Item
    {
      std::atomic<uint64_t> version;
      T value;
    };

    std::vector<Item> items_;
    uint64_t capacity_;

    std::atomic<uint64_t> head_;
    std::atomic<uint64_t> tail_;
};

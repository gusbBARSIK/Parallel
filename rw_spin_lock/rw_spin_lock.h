#pragma once

#include <atomic>
#include <thread>


class RWSpinLock {
 public:
  RWSpinLock() {
  }

  void LockRead() {
    for (;;) {
      auto check_for_wait = counter_.load();
      if (check_for_wait & 1) {
        std::this_thread::yield();
        continue;
      }
      auto change_counter = check_for_wait;
      change_counter += 2;
      if (counter_.compare_exchange_weak(check_for_wait, change_counter)) {
        break;
      }
    }
  }

  void UnlockRead() {
      counter_ -= 2;
  }

  void LockWrite() {
    for (;;) {
        counter_ |= 1;
        while (counter_.load() != 1) {
            std::this_thread::yield();
            counter_.fetch_or(1);
        }
        uint64_t joke_for_CAS = 1;
        if (counter_.compare_exchange_weak(joke_for_CAS, 3)) {
            break;
        } else {
            std::this_thread::yield();
        }
    }
  }

  void UnlockWrite() {
    counter_.store(0);
  }

 private:
    std::atomic<uint64_t> counter_ = 0;
};

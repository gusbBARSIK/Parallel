#pragma once

#include <atomic>
#include <optional>
#include <mutex>
#include <exception>
#include <condition_variable>


template <typename T>
class UnbufferedChannel{
 public:
  UnbufferedChannel() {
    closed_ = false;
    write_down_ = false;
  }

  void Send(const T& value) {
    std::unique_lock s_locker(for_send_);
    std::unique_lock closer(for_block_);
    if (closed_) {
      throw std::runtime_error("Channel is closed");
    }
    write_down_ = true;
    val = &value;
    waiter_rec_.notify_one();
    while(!(closed_|| !write_down_)) {
      waiter_send_.wait(closer);
    }
    if (write_down_) {
      throw std::runtime_error("Channel is closed");
    }
  }

  std::optional<T> Recv() {
    std::unique_lock r_locker(for_rec_);
    std::unique_lock closer(for_block_);
    while(!(closed_|| write_down_)) {
      waiter_rec_.wait(closer);
    }
    if (closed_) {
      return std::nullopt;
    }
    write_down_ = false;
    std::optional<T> res(*val);
    waiter_send_.notify_one();
    return res;
  }

  void Close() {
    std::unique_lock closer(for_block_);
    closed_ = true;
    waiter_rec_.notify_all();
    waiter_send_.notify_all();
  }

 private: 
    T const *val;
    bool closed_;
    bool write_down_;
    std::mutex for_block_;
    std::mutex for_send_;
    std::mutex for_rec_;
    std::condition_variable waiter_rec_;
    std::condition_variable waiter_send_;
};


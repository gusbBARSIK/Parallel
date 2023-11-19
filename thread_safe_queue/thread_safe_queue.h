#include <iostream>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <thread>
#include <optional>
#include <queue>


template <typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() {
  }

  void Push(const T& value) {
    std::unique_lock<std::mutex> mute_var(mute_);
    queue_.push(value);
    cv_.notify_one();
  }

  T Pop() {
    std::unique_lock<std::mutex> mute_var(mute_);
    
    cv_.wait(mute_var, [&](){return !queue_.empty();});

    auto value = queue_.front();
    queue_.pop();
    mute_var.unlock();
    return value;
  }

  std::optional<T> TryPop() {
    std::unique_lock<std::mutex> mute_var(mute_);
    if (queue_.empty()) {
      return std::nullopt;
    }
    auto value = queue_.front();
    queue_.pop();
    return value;
  }


 private:
  std::mutex mute_;
  std::condition_variable cv_;
  std::queue<T> queue_;
};

#pragma once

#include <mutex>


class Fork {
 public:
  Fork(size_t id) : id_(id) {
  }

  size_t Id() const {
    return id_;
  }

  void Get() {
    mutex_.lock();
  }

  bool TryGet() {
    return mutex_.try_lock();
  }

  void Put() {
    mutex_.unlock();
  }

 private:
  size_t id_;
  std::mutex mutex_;
};

class Philosopher {
 public:
  Philosopher(size_t id, Fork* first_fork, Fork* second_fork) 
      : id_(id)
      , first_fork_(first_fork)
      , second_fork_(second_fork)
  {
    if (first_fork_->Id() > second_fork_->Id()) {
        std::swap(first_fork_, second_fork_);
    }
  }

  size_t Id() const {
    return id_;
  }

  void Eat() {
    first_fork_->Get();
    second_fork_->Get();
  }

  void Think() {
    first_fork_->Put();
    second_fork_->Put();
  }

 private:
  const size_t id_;
  Fork* first_fork_;
  Fork* second_fork_;
};

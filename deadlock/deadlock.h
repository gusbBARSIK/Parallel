#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>


class Deadlock {
public:
  Deadlock() {
  }

  void ThreadOne() {
    std::unique_lock<std::mutex> first_mut(mutex1_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));;
    std::unique_lock<std::mutex> second_mut(mutex2_);
  }

  void ThreadTwo() {
    std::unique_lock<std::mutex> second_mut(mutex2_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::unique_lock<std::mutex> first_mut(mutex1_);
  }

private:
  std::mutex mutex1_;
  std::mutex mutex2_;
};

#pragma once

#include <ostream>
#include <string_view>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

class HelloWorld {
public:
  static constexpr std::string_view kHelloPrefix = "Hello, World! From thread ";

  HelloWorld(size_t n_threads)
    :n_threads_(n_threads)
  {}

  void SayHello(std::ostream& os) {
    std::mutex osmutex;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < n_threads_; ++i) {
      threads.emplace_back([&] () {
        std::unique_lock<std::mutex> locker(osmutex);
        os << kHelloPrefix << std::this_thread::get_id() << "\n";
      });
    }
    for (auto& t : threads) {
      t.join();
    }
  }

private:
  const size_t n_threads_;
};
#pragma once

#include <atomic>
#include <thread>

class TicketLock {
 public:
  TicketLock() {
    now_serving = 0;
    next_ticket = 0;
  }

  void Lock() {
    uint64_t my_ticket = next_ticket.fetch_add(1);
    while (now_serving != my_ticket) {
      std::this_thread::yield();   
    }
  }

  void Unlock() {
    ++now_serving;  
  }

 private:
  std::atomic<uint64_t> now_serving, next_ticket;
};

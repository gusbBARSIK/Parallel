#pragma once

#include <atomic>
#include <thread>


class MCSLock {

public:

    MCSLock() {
    }

    void Lock() {
        Node* prev = tail_.exchange(&node_);
        if (prev) {
            node_.acq.store(false);
            prev->nxt.store(&node_);
            while (!node_.acq.load()) {
                std::this_thread::yield();
            }
        }
    }

    void Unlock() {
        Node* node_ptr = &node_;
        if (tail_.compare_exchange_weak(node_ptr, nullptr)) {
          return;
        }
        while (!node_.nxt.load()) {;}
        node_.nxt.load()->acq.store(true);
    }

private:

    class Node {
    public:
       Node() : acq(true), nxt(nullptr) {}

    public:
       std::atomic<bool> acq;
       std::atomic<Node*> nxt;
    };

    std::atomic<Node*> tail_{nullptr};

    static thread_local inline Node node_;

};
#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <numeric>

template <typename RandomAccessIterator, class T, class Func>
T parallel_reduce(RandomAccessIterator begin, RandomAccessIterator end,
                  const T& initial_value, Func func, size_t n_threads) {

    std::vector<std::thread> threads;
    std::vector<T> result_thread(n_threads);
    for (size_t i = 0; i < n_threads; ++i) {
        const size_t size = (end - begin);
        RandomAccessIterator start = begin + (size / n_threads) * i;
        RandomAccessIterator finish = end;
        if (i != n_threads - 1) {
            finish = begin + (size / n_threads) * (i + 1);
        }
        threads.emplace_back([start, finish, &func,  initial_value, &result_thread, i] () {
            result_thread[i] = std::reduce(start, finish, initial_value, func);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return std::reduce(result_thread.begin(), result_thread.end(), initial_value, func);
}

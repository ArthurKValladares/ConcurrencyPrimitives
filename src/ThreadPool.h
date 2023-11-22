#pragma once

#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <future>

#include "ThreadSafeQueue.h"

#define NOOP_THREAD_POOL false
#define REFERENCE_THREAD_POOL false

#if NOOP_THREAD_POOL

struct ThreadPool {
    ThreadPool() {}

    void Schedule(const std::function<void()>);
    void Wait();
};

#else

#if !REFERENCE_THREAD_POOL

struct ThreadJoiner
{
    explicit ThreadJoiner(std::vector<std::thread>& threads) 
        : m_threads{ threads }
    {}

    ~ThreadJoiner() {
        for (std::thread& thread : m_threads) {
            if (thread.joinable()) { 
                thread.join(); 
            }
        }
    }

private:
    std::vector<std::thread>& m_threads;
};

struct ThreadPool {
    ThreadPool();
    ~ThreadPool();

    void Schedule(const std::function<void()>);
    void Wait();

private:
    void WorkerThread();

    ThreadSafeQueue<std::function<void()>> m_work_queue;
    std::vector<std::thread>               m_threads;
    ThreadJoiner                           m_joiner;
    std::atomic_bool                       m_done;
    std::vector<std::future<void>>         m_futures;
};

#else

#include "BS_thread_pool.hpp"

struct ThreadPool {
    ThreadPool() {}
    
    void Schedule(const std::function<void()>);
    void Wait();
private:
    BS::thread_pool m_pool;
};

#endif

#endif
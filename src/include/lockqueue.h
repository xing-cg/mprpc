#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// 异步写日志的日志队列
template<typename T>
class LockQueue
{
public:
    // 多个worker线程都将会调用
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_cv.notify_one();
    }
    // 一个Logger线程调用，一个线程等待日志队列有日志，pop返回给Logger
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            m_cv.wait(lock);
        }
        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};
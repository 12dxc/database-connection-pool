#include <condition_variable>
#include <cstddef>
#include <expected>
#include <mutex>
#include <queue>

// 加锁的并发安全队列
template <typename T>
class ThreadSafeQueue
{
private:
    mutable std::mutex mutex_;
    std::queue<T> data_queue_;
    std::condition_variable cond_;

public:
    ThreadSafeQueue() = default;

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_queue_.push(std::move(new_value));
        cond_.notify_one();
    }

    T waitPop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]
                   { return !data_queue_.empty(); });
        data_queue_.pop();
        return std::move(data_queue_.front());
    }

    bool tryPop(T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_queue_.empty())
            return false;
        value = std::move(data_queue_.front());
        data_queue_.pop();
        return true;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_queue_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_queue_.size();
    }

    T front() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_queue_.front();
    }

    T back() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_queue_.back();
    }
};
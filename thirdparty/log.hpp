#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <format>
#include <mutex>
#include <print>
#include <queue>
#include <source_location>
#include <sstream>
#include <string>
#include <thread>

/* 日志级别 */
enum Level
{
    DEBUG, // 调试级别
    INFO,  // 提示级别
    WARN,  // 警告级别
    ERROR, // 错误级别
};

/* 处理日志级别工具类 */
class LogLevelUtil
{
  public:
    static const std::string ToString(Level level)
    {
        switch (level)
        {
#define XX(name)                                                                                                       \
    case name:                                                                                                         \
        return #name;                                                                                                  \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);

#undef XX
        default:
            return "Unknonwn level";
        }
    }

    static const std::string getColor(Level level)
    {
        return level_strings_.at(level);
    }

  private:
    // 日志等级打印字符串
    static constexpr std::array level_strings_{
        "\033[32m", /* 绿色 */
        "\033[36m", /* 青色 */
        "\033[31m", /* 红色 */
        "\033[41m", /* 红色背景 */
    };
};

/* 日志器 */
class Logger
{
  public:
    ~Logger()
    {
        done_ = true;
        cond_.notify_one();
        write_thread_.join();
    }

    static Logger &getInstance()
    {
        static Logger logger;
        return logger;
    }

    void log(Level level, const std::source_location &sl)
    {
        std::lock_guard lock(mutex_);
        level_ = level;
        sl_ = sl;

        time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        // 格式化日志等级
        ss_ << std::format("{}[{}]{} ", LogLevelUtil::getColor(level_), LogLevelUtil::ToString(level_), "\033[0m");

        // 格式化错误信息
        ss_ << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
            << std::format(" {}:{}> ", sl.file_name(), sl.line());
    }

    // 线程安全的日志推送
    void pushToQueue()
    {
        std::lock_guard lock(mutex_);
        que_.push(ss_.str());
        ss_.clear();
        ss_.str("");
        cond_.notify_one();
    }

    void processQueue()
    {
        while (!done_)
        {
            std::unique_lock lock(mutex_);
            cond_.wait(lock, [this]() { return !que_.empty(); });

            while (!que_.empty() && !done_)
            {
                std::println("{}", que_.front());
                que_.pop();
            }
        }
    }

    std::stringstream &getSS()
    {
        return ss_;
    }

  private:
    Logger()
    {
        write_thread_ = std::thread(&Logger::processQueue, this);
    }

    std::stringstream ss_;
    Level level_{DEBUG};
    std::source_location sl_;

    // 异步所需变量
    std::thread write_thread_;
    std::queue<std::string> que_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> done_{false}; // 控制线程循环
};

inline std::stringstream &LOG(Level level, std::source_location sl = std::source_location::current())
{
    auto &logger = Logger::getInstance();
    logger.log(level, sl);
    return logger.getSS();
}

inline char logEnd()
{
    Logger::getInstance().pushToQueue();
    return '\0';
}

#define LOG_DEBUG LOG(DEBUG)
#define LOG_INFO LOG(INFO)
#define LOG_WARN LOG(WARN)
#define LOG_ERROR LOG(ERROR)

#define ENDL logEnd()

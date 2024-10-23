#pragma once

#include <array>
#include <chrono>
#include <ctime>
#include <format>
#include <print>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>

namespace minilog
{
    namespace detail
    {
        // 日志等级
        enum class LogLevel
        {
            DEBUG,
            INFO,
            ERROR,
            FATAL,
        };

        // 日志等级打印字符串
        const std::array<std::string_view, sizeof(LogLevel)> level_strings = {
            "DEBUG",
            "INFO",
            "ERROR",
            "FATAL",
        };

        // 日志等级颜色
        const std::array<std::string_view, sizeof(LogLevel)> level_colors = {
            "\033[32m", // 绿色
            "\033[36m", // 青色
            "\033[31m", // 红色
            "\033[41m", // 红色背景
        };

        template <LogLevel level>
        inline void logImpl(const std::string &msg, const std::source_location &sl)
        {
            using namespace std;

            ostringstream oss;
            const auto now = chrono::system_clock::now();
            const time_t t_c = chrono::system_clock::to_time_t(now);
            oss << put_time(localtime(&t_c), "%Y-%m-%d %H:%M:%S");

            // 格式化日志等级
            string str_level = format(
                "{}[{}]{}",
                level_colors[(uint8_t)level],
                level_strings[(uint8_t)level],
                "\033[0m");

            // 格式化错误信息
            string str_message = format(
                "{}:{} {} : {}",
                sl.file_name(),
                sl.line(),
                oss.str(),
                msg);

            println("{} {}", str_level, str_message);
        }
    } // namespace detail

    inline void debug(std::string msg, std::source_location sl = std::source_location::current())
    {
        detail::logImpl<detail::LogLevel::DEBUG>(msg, sl);
    }

    inline void info(std::string msg, std::source_location sl = std::source_location::current())
    {
        detail::logImpl<detail::LogLevel::INFO>(msg, sl);
    }

    inline void error(std::string msg, std::source_location sl = std::source_location::current())
    {
        detail::logImpl<detail::LogLevel::ERROR>(msg, sl);
    }

    inline void fatal(std::string msg, std::source_location sl = std::source_location::current())
    {
        detail::logImpl<detail::LogLevel::FATAL>(msg, sl);
    }
};

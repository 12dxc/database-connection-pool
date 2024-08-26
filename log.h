#pragma once

#include <chrono>
#include <ctime>
#include <format>
#include <print>
#include <source_location>
#include <sstream>
#include <string_view>

namespace minilog
{
    namespace detail
    {
        // 日志等级
        enum class LogLevel : uint8_t
        {
            DEBUG,
            INFO,
            ERROR,
            FATAL,
        };

        // 日志等级打印字符串
        constexpr std::string_view level_strings[] = {
            "DEBUG",
            "INFO",
            "ERROR",
            "FATAL",
        };

        // 日志等级颜色
        constexpr const char *level_colors[] = {
            "\033[32m", // 绿色
            "\033[36m", // 青色
            "\033[31m", // 红色
            "\033[41m", // 红色背景
        };

        template <LogLevel level>
        inline void logImpl(std::string msg, std::source_location sl)
        {
            auto logEntry = [&]()
            {
                std::ostringstream ss;
                const auto now = std::chrono::system_clock::now();
                const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
                ss << std::put_time(localtime(&t_c), "%Y-%m-%d %H:%M:%S");

                // 格式化日志等级
                std::string str_level =
                    std::format("{}[{}]{}",
                                level_colors[(uint8_t)level],
                                level_strings[(uint8_t)level],
                                "\033[0m");

                // 格式化错误信息
                std::string str_message =
                    std::format("{}:{} {} : {}",
                                sl.file_name(),
                                sl.line(),
                                ss.str(),
                                msg);

                println("{} {}", str_level, str_message);
            };
            logEntry();
        }
    }

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

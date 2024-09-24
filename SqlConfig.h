#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <sys/types.h>
#include <unordered_map>

/* 数据库配置信息类 */
class SqlConfig
{
public:
    SqlConfig() = default;
    // 从配置文件中加载配置项
    auto loadConfigFile(const std::string &filename = "mysql.ini") -> bool;

    auto ip() -> std::string;
    auto port() const -> uint16_t;
    auto username() -> std::string;
    auto passwd() -> std::string;
    auto dbname() -> std::string;

    // 初始连接量
    auto init_size() const -> int;
    // 最大连接量
    auto max_size() const -> int;
    // 最大空闲时间
    auto max_idle_time() const -> int;
    // 获取连接的超时时间
    auto conn_timeout() const -> int;

private:
    // mysql的连接信息
    std::string ip_;
    uint16_t port_{};
    std::string username_;
    std::string passwd_;
    std::string dbname_;

    // mysql的连接配置项
    int init_size_{};     // 初始连接量
    int max_size_{};      // 最大连接量
    int max_idle_time_{}; // 最大空闲时间
    int conn_timeout_{};  // 获取连接的超时时间

    // 配置项映射表，用于简化操作
    std::unordered_map<std::string, std::function<void(const std::string &)>> config_handlers_ = {
        {"ip", [&](const std::string &val)
         { ip_ = val; }},
        {"port", [&](const std::string &val)
         { port_ = std::stoi(val); }},
        {"username", [&](const std::string &val)
         { username_ = val; }},
        {"password", [&](const std::string &val)
         { passwd_ = val; }},
        {"dbname", [&](const std::string &val)
         { dbname_ = val; }},
        {"initSize", [&](const std::string &val)
         { init_size_ = std::stoi(val); }},
        {"maxSize", [&](const std::string &val)
         { max_size_ = std::stoi(val); }},
        {"maxIdleTime", [&](const std::string &val)
         { max_idle_time_ = std::stoi(val); }},
        {"connectionTimeOut", [&](const std::string &val)
         { conn_timeout_ = std::stoi(val); }},
    };
};
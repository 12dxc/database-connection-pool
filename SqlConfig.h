#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

/* 数据库配置信息类 */
class SqlConfig
{
public:
    SqlConfig() = default;
    // 从配置文件中加载配置项
    auto loadConfigFile(const std::string &filename = "mysql.ini") -> bool
    {
        FILE *pf = nullptr;
        pf = fopen(filename.c_str(), "re");
        if (pf == nullptr)
        {
            perror("fopen");
            return true;
        }

        while (feof(pf) == 0)
        {
            char line[1024] = {};
            fgets(line, 1024, pf);
            std::string str = line;
            int idx = str.find('=', 0);
            if (idx == -1)
            { // 无效配置项
                continue;
            }
            size_t end_idx = str.find('\n', idx);
            std::string key = str.substr(0, idx);
            std::string val = str.substr(idx + 1, end_idx - idx - 1);

            auto it = config_handlers_.find(key);
            if (it != config_handlers_.end())
                it->second(val);
        }

        return true;
    }

    auto ip() -> std::string { return ip_; }
    auto port() const -> uint16_t { return port_; }
    auto username() -> std::string { return username_; }
    auto passwd() -> std::string { return passwd_; }
    auto dbname() -> std::string { return dbname_; }

    // 初始连接量
    auto init_size() const -> int { return init_size_; }
    // 最大连接量
    auto max_size() const -> int { return max_size_; }
    // 最大空闲时间
    auto max_idle_time() const -> int { return max_idle_time_; }
    // 获取连接的超时时间
    auto conn_timeout() const -> int { return conn_timeout_; }

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
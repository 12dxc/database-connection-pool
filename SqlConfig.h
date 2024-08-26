#pragma once
#include "Connection.h"
#include <functional>
#include <string>
#include <unordered_map>

/* 数据库配置信息类 */
class SqlConfig
{
  public:
    SqlConfig() = default;
    // 从配置文件中加载配置项
    bool loadConfigFile();

    std::string ip();
    unsigned short port();
    std::string username();
    std::string passwd();
    std::string dbname();
    // 初始连接量
    int init_size();
    // 最大连接量
    int max_size();
    // 最大空闲时间
    int max_idle_time();
    // 获取连接的超时时间
    int conn_timeout();

  private:
    // mysql的连接信息
    std::string ip_;
    unsigned short port_;
    std::string username_;
    std::string passwd_;
    std::string dbname_;

    // mysql的连接配置项
    int init_size_;     // 初始连接量
    int max_size_;      // 最大连接量
    int max_idle_time_; // 最大空闲时间
    int conn_timeout_;  // 获取连接的超时时间

    // 配置项映射表，用于简化操作
    std::unordered_map<std::string, std::function<void(const std::string &)>> config_handlers_ = {
        {"ip", [&](const std::string &val) { ip_ = val; }},
        {"port", [&](const std::string &val) { port_ = std::stoi(val); }},
        {"username", [&](const std::string &val) { username_ = val; }},
        {"password", [&](const std::string &val) { passwd_ = val; }},
        {"dbname", [&](const std::string &val) { dbname_ = val; }},
        {"initSize", [&](const std::string &val) { init_size_ = std::stoi(val); }},
        {"maxSize", [&](const std::string &val) { max_size_ = std::stoi(val); }},
        {"maxIdleTime", [&](const std::string &val) { max_idle_time_ = std::stoi(val); }},
        {"connectionTimeOut", [&](const std::string &val) { conn_timeout_ = std::stoi(val); }},
    };
};
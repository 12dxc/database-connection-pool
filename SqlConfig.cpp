#include "SqlConfig.h"
#include <cstddef>
using std::string;

// 从配置文件中加载配置项
auto SqlConfig::loadConfigFile(const string &filename) -> bool
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
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1)
        { // 无效配置项
            continue;
        }
        size_t end_idx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string val = str.substr(idx + 1, end_idx - idx - 1);

        auto it = config_handlers_.find(key);
        if (it != config_handlers_.end())
            it->second(val);
    }

    return true;
}

auto SqlConfig::ip() -> string
{
    return ip_;
}
auto SqlConfig::port() const -> uint16_t
{
    return port_;
}
auto SqlConfig::username() -> string
{
    return username_;
}
auto SqlConfig::passwd() -> string
{
    return passwd_;
}
auto SqlConfig::dbname() -> string
{
    return dbname_;
}
// 初始连接量
auto SqlConfig::init_size() const -> int
{
    return init_size_;
}
// 最大连接量
auto SqlConfig::max_size() const -> int
{
    return max_size_;
}
// 最大空闲时间
auto SqlConfig::max_idle_time() const -> int
{
    return max_idle_time_;
}
// 获取连接的超时时间
auto SqlConfig::conn_timeout() const -> int
{
    return conn_timeout_;
}

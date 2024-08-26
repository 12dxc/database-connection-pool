#include "SqlConfig.h"
using std::string;

// 从配置文件中加载配置项
bool SqlConfig::loadConfigFile()
{
    FILE *pf = nullptr;
    pf = fopen("mysql.ini", "r");
    if (pf == nullptr)
    {
        perror("fopen");
        return 1;
    }

    while (!feof(pf))
    {
        char line[1024] = {};
        fgets(line, 1024, pf);
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1) // 无效配置项
            continue;
        int end_idx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string val = str.substr(idx + 1, end_idx - idx - 1);

        auto it = config_handlers_.find(key);
        if (it != config_handlers_.end())
            it->second(val);
    }

    return true;
}

string SqlConfig::ip()
{
    return ip_;
}
unsigned short SqlConfig::port()
{
    return port_;
}
string SqlConfig::username()
{
    return username_;
}
string SqlConfig::passwd()
{
    return passwd_;
}
string SqlConfig::dbname()
{
    return dbname_;
}
// 初始连接量
int SqlConfig::init_size()
{
    return init_size_;
}
// 最大连接量
int SqlConfig::max_size()
{
    return max_size_;
}
// 最大空闲时间
int SqlConfig::max_idle_time()
{
    return max_idle_time_;
}
// 获取连接的超时时间
int SqlConfig::conn_timeout()
{
    return conn_timeout_;
}

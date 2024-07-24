#pragma once
#include <mysql/mysql.h>
#include <string>

/* 数据库操作类 */
class Connection
{
  public:
    // 初始化数据库连接
    Connection();

    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    bool connect(const std::string &ip, const std::string &user, const std::string &passwd, const std::string &dbname,
                 unsigned short port);

    // 更新操作 insert、delete、update
    bool update(const std::string &sql);

    // 查询操作 select
    MYSQL_RES *select(const std::string &sql);

    // 刷新以下连接的起始空闲时间点
    void refreshAliveTime()
    {
        alivetime_ = clock();
    }
    // 返回存活的时间
    clock_t getAliveTime() const
    {
        return clock() - alivetime_;
    }

  private:
    MYSQL *conn_;       // 表示和Connection Server的一条连接
    clock_t alivetime_; // 记录进入空闲状态后的存活起始时间
};
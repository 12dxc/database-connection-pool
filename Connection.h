#pragma once
#include "SqlConfig.h"
#include <ctime>
#include <mysql/mysql.h>
#include <string>

using namespace std;

/*
实现MySQL数据库的操作
*/
class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    auto connect(SqlConfig &sqlConfig) -> bool;
    // 更新操作 insert、delete、update
    auto update(const string &sql) -> bool;
    // 查询操作 select
    auto query(const string &sql) -> MYSQL_RES *;

    // 刷新一下连接的起始的空闲时间点
    void refreshAliveTime()
    {
        alivetime_ = clock();
    }
    // 返回存活的时间
    [[nodiscard]] auto getAliveTime() const -> clock_t
    {
        return clock() - alivetime_;
    }

    // 复制移动删除构造函数
    Connection(Connection &) = delete;
    Connection(Connection &&) = delete;
    auto operator=(const Connection &) = delete;
    auto operator=(const Connection &&) = delete;

private:
    MYSQL *conn_;       // 表示和MySQL Server的一条连接
    clock_t alivetime_; // 记录进入空闲状态后的起始存活时间
};
#pragma once
#include "SqlConfig.h"
#include "log.h"
#include <ctime>
#include <mysql/mysql.h>
#include <string>

/*
实现MySQL数据库的操作
*/
class Connection
{
public:
    // 初始化数据库连接
    Connection()
        : conn_(mysql_init(nullptr)),
          alivetime_(clock())
    {
    }
    // 释放数据库连接资源
    ~Connection()
    {
        if (conn_ != nullptr)
        {
            mysql_close(conn_);
        }
    }

    // 连接数据库
    auto connect(SqlConfig &sqlConfig) -> bool
    {
        MYSQL *flag = mysql_real_connect(
            conn_,
            sqlConfig.ip().c_str(),
            sqlConfig.username().c_str(),
            sqlConfig.passwd().c_str(),
            sqlConfig.dbname().c_str(),
            sqlConfig.port(),
            nullptr, 0);
        return flag != nullptr;
    }

    // 更新操作 insert、delete、update
    auto update(const std::string &sql) -> bool
    {
        if (mysql_query(conn_, sql.c_str()) != 0)
        {
            minilog::info("更新失败:" + sql);
            return false;
        }
        return true;
    }

    // 查询操作 select
    auto query(const std::string &sql) -> MYSQL_RES *
    {
        if (mysql_query(conn_, sql.c_str()) != 0)
        {
            minilog::info("查询失败:" + sql);
            return nullptr;
        }
        return mysql_use_result(conn_);
    }

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
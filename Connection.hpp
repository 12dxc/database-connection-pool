#pragma once
#include "SqlConfig.hpp"
#include "log.hpp"
#include <ctime>
#include <mysql/mysql.h>
#include <print>
#include <string>

/*
实现MySQL数据库的操作
*/
class Connection
{
public:
    // 初始化数据库连接
    Connection()
    {
        conn_ = mysql_init(nullptr);
        if (conn_ == nullptr)
            LOG_ERROR << "初始化失败" << ENDL;
    }
    // 释放数据库连接资源
    ~Connection()
    {
        if (conn_ != nullptr)
        {
            mysql_close(conn_);
            conn_ = nullptr;
        }
    }

    auto &conn() { return conn_; }

    // 连接数据库
    bool connect(const SqlConfig &sql_config)
    {
        MYSQL *flag = mysql_real_connect(
            conn_,
            sql_config.ip().c_str(),
            sql_config.username().c_str(),
            sql_config.password().c_str(),
            sql_config.dbname().c_str(),
            sql_config.port(),
            nullptr, 0);
        if (flag == nullptr)
            LOG_INFO << "连接失败: " << mysql_error(conn_) << ENDL;
        return flag != nullptr;
    }

    // 更新操作 insert、delete、update
    bool update(const std::string &sql)
    {
        if (mysql_query(conn_, sql.c_str()))
        {
            LOG_INFO << "更新失败: " << mysql_error(conn_) << "\nsql语句:" << sql << ENDL;
            return false;
        }
        return true;
    }

    // 查询操作 select
    MYSQL_RES *query(const std::string &sql)
    {
        if (mysql_query(conn_, sql.c_str()))
        {
            LOG_INFO << "查询失败: " << mysql_error(conn_) << "\nsql语句:" << sql << ENDL;
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
    [[nodiscard]] clock_t getAliveTime() const
    {
        return clock() - alivetime_;
    }

private:
    MYSQL *conn_{nullptr}; // 表示和MySQL Server的一条连接
    clock_t alivetime_{0}; // 记录进入空闲状态后的起始存活时间
};
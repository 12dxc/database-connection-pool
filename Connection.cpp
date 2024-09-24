#include "Connection.h"
#include "log.h"
using namespace std;

// 初始化数据库连接
Connection::Connection()
    : conn_(mysql_init(nullptr)),
      alivetime_(clock())
{
}

Connection::~Connection()
{
    // 释放数据库连接资源
    if (conn_ != nullptr)
    {
        mysql_close(conn_);
    }
}

auto Connection::connect(SqlConfig &sqlConfig) -> bool
{
    // 连接数据库
    MYSQL *p =
        mysql_real_connect(conn_,
                           sqlConfig.ip().c_str(),
                           sqlConfig.username().c_str(),
                           sqlConfig.passwd().c_str(),
                           sqlConfig.dbname().c_str(),
                           sqlConfig.port(),
                           nullptr, 0);
    return p != nullptr;
}

auto Connection::update(const string &sql) -> bool
{
    // 更新操作 insert、delete、update
    if (mysql_query(conn_, sql.c_str()) != 0)
    {
        minilog::info("更新失败:" + sql);
        return false;
    }
    return true;
}

auto Connection::query(const string &sql) -> MYSQL_RES *
{
    // 查询操作 select
    if (mysql_query(conn_, sql.c_str()) != 0)
    {
        minilog::info("查询失败:" + sql);
        return nullptr;
    }
    return mysql_use_result(conn_);
}
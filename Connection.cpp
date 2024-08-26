#include "Connection.h"
#include "log.h"
using namespace std;

Connection::Connection()
{
    // 初始化数据库连接
    _conn = mysql_init(nullptr);
}

Connection::~Connection()
{
    // 释放数据库连接资源
    if (_conn != nullptr)
        mysql_close(_conn);
}

bool Connection::connect(SqlConfig &sqlConfig)
{
    // 连接数据库
    MYSQL *p =
        mysql_real_connect(_conn,
                           sqlConfig.ip().c_str(),
                           sqlConfig.username().c_str(),
                           sqlConfig.passwd().c_str(),
                           sqlConfig.dbname().c_str(),
                           sqlConfig.port(),
                           nullptr, 0);
    return p != nullptr;
}

bool Connection::update(string sql)
{
    // 更新操作 insert、delete、update
    if (mysql_query(_conn, sql.c_str()))
    {
        minilog::info("更新失败:" + sql);
        return false;
    }
    return true;
}

MYSQL_RES *Connection::query(string sql)
{
    // 查询操作 select
    if (mysql_query(_conn, sql.c_str()))
    {
        minilog::info("查询失败:" + sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}
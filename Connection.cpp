#include "Connection.h"
#include "public.h"
using std::string;

// 初始化数据库连接
Connection::Connection()
{
    conn_ = mysql_init(nullptr);
}

// 释放数据库连接资源
Connection::~Connection()
{
    if (conn_ != nullptr)
        mysql_close(conn_);
}

// 连接数据库
bool Connection::connect(const string &ip, const string &user, const string &passwd, const string &dbname,
                         unsigned short port)
{
    MYSQL *p = mysql_real_connect(conn_, ip.c_str(), user.c_str(), passwd.c_str(), dbname.c_str(), port, nullptr, 0);
    return p != nullptr;
}

// 更新操作 insert、delete、update
bool Connection::update(const string &sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG("更新失败" + sql);
        return false;
    }
    return true;
}

// 查询操作 select
MYSQL_RES *Connection::select(const string &sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG("查询失败" + sql);
        return nullptr;
    }
    return mysql_use_result(conn_);
}
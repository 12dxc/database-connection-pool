#pragma once

#include "Connection.hpp"

#include <atomic>
#include <condition_variable>
#include <ctime>
#include <memory>
#include <mutex>
#include <mysql/mysql.h>
#include <queue>

/* 数据库连接池类 */
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool &getConnectionPool();

    // 给外部提供接口，从连接池获取一个可用的空闲连接
    std::shared_ptr<Connection> getConnection();

private:
    // 单例，构造函数私有化
    ConnectionPool();

    // 运行在独立的线程中，专门负责生成新连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
    void scannerConnectionTask();

private:
    SqlConfig sql_config_;              // sql的配置类
    std::queue<Connection *> conn_que_; // 存储MySQL连接的队列
    std::mutex que_mutex_;              // 维护连接队列线程安全的互斥锁
    std::atomic<int> conn_cnt_;         // 记录连接所创建的Connection总数量
    std::condition_variable cv_;        // 设置条件变量，用于连接生产线程和消费线程的通信
};

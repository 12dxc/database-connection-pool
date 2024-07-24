#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include "Connection.h"
#include <atomic>
#include <condition_variable>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>
#include <mysql/mysql.h>
#include <queue>
#include <string>
#include <thread>

/* 数据库连接池类 */
class ConnectionPool
{
  public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();

    // 给外部提供接口，从连接池获取一个可用的空闲连接
    std::shared_ptr<Connection> getConnection();

  private:
    // 单例#1  构造函数私有化
    ConnectionPool();

    // 从配置文件中加载配置项
    bool loadConfigFile();

    // 运行在独立的线程中，专门负责生成新连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
    void scannerConnectionTask();

    std::string ip_;       // MySQL的ip地址
    unsigned short port_;  // MySQL的端口号  3369
    std::string username_; // MySQL登录用户名
    std::string passwd_;   // MySQL登录密码
    std::string dbname_;   // 连接的数据库名称
    int initSize_;         // 初始连接量
    int maxSize_;          // 最大连接量
    int maxIdleTime_;      // 最大空闲时间
    int connTimeout_;      // 获取连接的超时时间

    std::queue<Connection *> connQue_; // 存储MySQL连接的队列
    std::mutex queMutex_;              // 维护连接队列线程安全的互斥锁
    std::atomic_int connCnt_;          // 记录连接所创建的Connection总数量
    std::condition_variable cv;        // 设置条件变量，用于连接生产线程和消费线程的通信
};

#endif
#include "ConnectionPool.h"

#include <climits>
#include <mutex>
#include <thread>

// 获取连接池对象实例
ConnectionPool &ConnectionPool::getConnectionPool()
{
    // C++11局部静态变量只会初始化一次，以此来实现单例
    static ConnectionPool pool;
    return pool;
}

// 连接池的构造
ConnectionPool::ConnectionPool()
{
    // 加载配置项
    if (!sql_config_.loadConfigFile())
        LOG_INFO << "数据库配置加载失败" << ENDL;

    // 创建初始数量的连接
    for (int i = 0; i != sql_config_.initSize(); ++i)
    {
        auto p = new Connection();
        p->connect(sql_config_);
        p->refreshAliveTime(); // 刷新开始空闲的起始时间
        conn_que_.push(p);
        ++conn_cnt_;
    }

    // 启动一个新的线程，作为连接的生产者
    std::thread produce([this]
                        { produceConnectionTask(); });
    produce.detach();
    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
    std::thread scanner([this]
                        { scannerConnectionTask(); });
    scanner.detach();
}

// 给外部提供接口，从连接池获取一个可用的空闲连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock lock(que_mutex_);
    while (conn_que_.empty())
        if (std::cv_status::timeout ==
            cv_.wait_for(lock, std::chrono::milliseconds(sql_config_.connectionTimeOut())))
            if (conn_que_.empty())
            {
                LOG_INFO << "获取空闲连接超时了...获取连接失败!" << ENDL;
                return nullptr;
            }

    // 自定义智能指针删除器，让其不要调用析构函数而是放回池中继续使用
    std::shared_ptr<Connection> sp(
        conn_que_.front(),
        [&](Connection *pcon)
        {
            std::unique_lock lock(que_mutex_);
            pcon->refreshAliveTime();
            conn_que_.push(pcon);
        });

    conn_que_.pop();
    cv_.notify_all(); // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接

    return sp;
}

// 运行在独立的线程中，专门负责生成新连接
void ConnectionPool::produceConnectionTask()
{
    while (true)
    {
        std::unique_lock lock(que_mutex_);
        while (!conn_que_.empty())
            cv_.wait(lock); // 队列不空，此处生产者线程进入等待状态

        // 连接数量未达到上限，继续创建新连接
        if (conn_cnt_ < sql_config_.maxSize())
        {
            auto *p = new Connection();
            p->connect(sql_config_);
            p->refreshAliveTime();
            conn_que_.push(p);
            ++conn_cnt_;
        }

        // 通知消费者线程，可以消费连接了
        cv_.notify_all();
    }
}

// 扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
void ConnectionPool::scannerConnectionTask()
{
    while (true)
    {
        // 通过sleep模拟定时器
        std::this_thread::sleep_for(std::chrono::seconds(sql_config_.maxIdleTime()));

        // 扫描整个队列，释放多余的连接
        std::unique_lock lock(que_mutex_);
        while (conn_cnt_ > sql_config_.initSize())
        {
            auto p = conn_que_.front();
            if (p->getAliveTime() >= (sql_config_.maxIdleTime() * 1000))
            {
                conn_que_.pop();
                --conn_cnt_;
                delete p; // 调用~Connection()释放连接
            }
            else
                break;
        }
    }
}

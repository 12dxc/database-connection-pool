#include "ConnectionPool.h"
#include "public.h"

#include <cerrno>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string>

using std::string;

// 获取连接池对象实例  线程安全的懒汉单例模式
ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool; // 静态自动进行lock和unlock
    return &pool;
}

// 连接池的构造
ConnectionPool::ConnectionPool()
{
    // 加载配置项了
    if (!sqlconfig_.loadConfigFile())
        return;

    // 创建初始数量的连接
    for (int i = 0; i != sqlconfig_.init_size(); ++i)
    {
        Connection *p = new Connection();
        p->connect(sqlconfig_.ip(), sqlconfig_.port(), sqlconfig_.username(), sqlconfig_.passwd(), sqlconfig_.dbname());

        p->refreshAliveTime(); // 刷新开始空闲的起始时间
        conn_que_.push(p);
        ++conn_cnt_;
    }

    // 启动一个新的线程，作为连接的生产者
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

// 运行在独立的线程中，专门负责生成新连接
void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(que_mutex_);
        while (!conn_que_.empty())
            cond_.wait(lock); // 队列不空，此处生产者线程进入等待状态

        // 连接数量未达到上限，继续创建新连接
        if (conn_cnt_ < sqlconfig_.max_size())
        {
            Connection *p = new Connection();
            p->connect(sqlconfig_.ip(), sqlconfig_.port(), sqlconfig_.username(), sqlconfig_.passwd(), sqlconfig_.dbname());
            p->refreshAliveTime();
            conn_que_.push(p);
            ++conn_cnt_;
        }

        // 通知消费者线程，可以消费连接了
        cond_.notify_all();
    }
}

// 给外部提供接口，从连接池获取一个可用的空闲连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(que_mutex_);
    // 等待直到队列非空或超时
    while (conn_que_.empty())
    {
        if (std::cv_status::timeout == cond_.wait_for(lock, std::chrono::milliseconds(sqlconfig_.conn_timeout())))
        {
            if (conn_que_.empty())
            {
                LOG("获取空闲连接超时了...获取连接失败!");
                return nullptr;
            }
        }
    }

    // 自定义智能指针删除器，让其不要调用析构函数而是放回池中继续使用
    std::shared_ptr<Connection> sp(conn_que_.front(), [&](Connection *pcon)
                                   {
        std::unique_lock<std::mutex> lock(que_mutex_);
        pcon->refreshAliveTime();
        conn_que_.push(pcon); });
    conn_que_.pop();
    cond_.notify_all(); // 消费完连接后，通知生产者线程

    return sp;
}

// 扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        // 通过sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(sqlconfig_.max_idle_time()));
        // 扫描整个队列，释放多余的连接
        std::unique_lock<std::mutex> lock(que_mutex_);
        while (conn_cnt_ > sqlconfig_.init_size())
        {
            Connection *p = conn_que_.front();
            if (p->getAliveTime() > (sqlconfig_.max_idle_time() * 1000))
            {
                conn_que_.pop();
                --conn_cnt_;
                delete p; // 调用~Connection()释放连接
            }
            else
            {
                break; // 队头的连接都没超过maxIdleTime_，其他连接肯定也没有
            }
        }
    }
}

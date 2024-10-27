#include "ConnectionPool.h"

using namespace std;

// sql指令
const string sql = "INSERT INTO `fruits` VALUES ('a1', 101, 'apple', 5.20);";

int main()
{
    clock_t begin = clock();

    // 单线程连接对比
#if 1
    ConnectionPool &cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 10000; ++i)
    {
        // 使用连接池测试
        std::shared_ptr<Connection> sp = cp.getConnection();
        sp->update(sql);
    }
#endif
    // 四线程连接对比
#if 0
    std::thread t1(mysql_update);
    std::thread t2(mysql_update);
    std::thread t3(mysql_update);
    std::thread t4(mysql_update);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
#endif

    clock_t end = clock();
    std::println("{}ms", end - begin);

    return 0;
}

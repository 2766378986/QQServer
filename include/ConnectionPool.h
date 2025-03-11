#ifndef USEDB_H
#define USEDB_H
#include <QObject>
#include <QSqlDatabase>
#include <QQueue>
#include <QMutex>
#include <QString>

class ConnectionPool {
public:
    static ConnectionPool * instance(); // 获取单例实例
    QSqlDatabase getConnection();  // 从连接池中获取一个连接
    void releaseConnection(QSqlDatabase db); // 释放连接
    void setMaxConnections(int max); // 设置最大连接数
    int getMaxConnections(); // 获取最大连接数
    QSqlDatabase createConnection(); // 创建一个新连接

private:
    ConnectionPool(); // 私有构造函数
    ~ConnectionPool(); // 私有析构函数

private:
    static ConnectionPool * connectionPool; // 单例实例
    int maxConnections; // 最大连接数
    QQueue<QSqlDatabase> pool; // 连接池
    QMutex mutex; // 互斥锁
    QString dbName = "server.db"; // 数据库名称
    int connectionCount = 0; // 当前连接个数
};

#endif // USEDB_H
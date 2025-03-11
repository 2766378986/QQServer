#include <ConnectionPool.h>
#include <QDebug>
#include <QSqlError>
#include <QMutexLocker>
#include <QSqlQuery>

ConnectionPool * ConnectionPool::connectionPool = nullptr;

ConnectionPool::ConnectionPool() : maxConnections(10) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "数据库连接失败" << db.lastError().text();
        return;
    }
    else {
         //检查和创建表的 SQL 语句
        QSqlQuery query(db);
        //创建用户表
        query.exec("CREATE TABLE IF NOT EXISTS Users ("
                   "qq_number VARCHAR(20) PRIMARY KEY NOT NULL, "
                   "password VARCHAR(255) NOT NULL, "
                   "avator LONGTEXT, "
                   "nickname VARCHAR(50), "
                   "signature TEXT, "
                   "gender TEXT CHECK (gender IN ('男', '女', '其他')), "
                   "question VARCHAR(255), "
                   "answer VARCHAR(255));");
        if (query.lastError().isValid()) {
            qDebug() << "创建用户表失败:" << query.lastError().text();
        }

        //创建聊天消息表
        query.exec("CREATE TABLE IF NOT EXISTS Messages ("
                   "message_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "sender_id VARCHAR(20) NOT NULL, "
                   "receiver_id VARCHAR(20) NOT NULL, "
                   "content LONGTEXT, "
                   "filename VARCHAT(20),"
                   "status VARCHAT(20),"
                   "timestamp TIMESTAMP, "
                   "message_type VARCHAR(20) NOT NULL, "
                   "FOREIGN KEY (sender_id) REFERENCES Users(qq_number) ON DELETE CASCADE);");
        if (query.lastError().isValid()) {
            qDebug() << "创建聊天消息表失败:" << query.lastError().text();
        }

        //创建好友关系表
        query.exec("CREATE TABLE IF NOT EXISTS Friends ("
                   "friendship_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "user_id VARCHAR(20) NOT NULL, "
                   "friend_id VARCHAR(20) NOT NULL, "
                   "FOREIGN KEY (user_id) REFERENCES Users(qq_number) ON DELETE CASCADE, "
                   "FOREIGN KEY (friend_id) REFERENCES Users(qq_number) ON DELETE CASCADE);");
        if (query.lastError().isValid()) {
            qDebug() << "创建好友关系表失败:" << query.lastError().text();
        }

        //创建申请表
        query.exec("CREATE TABLE IF NOT EXISTS FriendRequests ("
                   "request_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "sender_id VARCHAR(20) NOT NULL, "
                   "receiver_id INT NOT NULL, "
                   "request_type TEXT CHECK (request_type IN ('friend', 'group')) NOT NULL, "
                   "status TEXT CHECK (status IN ('pending', 'accepted', 'rejected')) NOT NULL DEFAULT 'pending', "
                   "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                   "FOREIGN KEY (sender_id) REFERENCES Users(qq_number) ON DELETE CASCADE);");
        if (query.lastError().isValid()) {
            qDebug() << "创建申请表失败:" << query.lastError().text();
        }

        qDebug() << "数据库表创建成功";
        db.close();
    }
}

ConnectionPool::~ConnectionPool() {
    while (!pool.isEmpty()) {
        QSqlDatabase db = pool.dequeue();
        db.close();
    }
}

QSqlDatabase ConnectionPool::createConnection() {
    if (pool.size() < maxConnections) {
        QString connectionName = QString("Connection_%1").arg(connectionCount++);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbName);
        pool.enqueue(db);

        if (!db.open()) {
            qDebug() << "创建新连接失败" << db.lastError().text();
            return QSqlDatabase();
        }
        return db;
    }
    else {
        qDebug() << "连接池已满，无法创建新连接";
        return QSqlDatabase();
    }
}


QSqlDatabase ConnectionPool::getConnection() {
    QMutexLocker locker(&mutex);

    while (!pool.isEmpty()) {
        QSqlDatabase db = pool.dequeue();
        if (db.isOpen()) {
            qDebug() << "获取现有连接";
            return db;
        }
        else {
            db.close();
            qDebug() << "连接已关闭，重新打开";
            // 如果连接已关闭，重新打开
            continue;
        }
    }

    // 如果连接池为空，创建新的连接
    return createConnection();
}

void ConnectionPool::releaseConnection(QSqlDatabase db) {
    QMutexLocker locker(&mutex);
    
    if (pool.size() < maxConnections) {
        pool.enqueue(db);
        qDebug() << "连接已返回连接池";
    }
    else {
        qDebug() << "连接池已满，无法返回连接";
        db.close();
    }
}

void ConnectionPool::setMaxConnections(int max) {
    QMutexLocker locker(&mutex);
    maxConnections = max;
    qDebug() << "最大连接数已设置为" << max;
}

int ConnectionPool::getMaxConnections() {
    QMutexLocker locker(&mutex);
    return maxConnections;
}


ConnectionPool * ConnectionPool::instance() {
    if (connectionPool == nullptr) {
        connectionPool = new ConnectionPool();
    }
    return connectionPool;
}


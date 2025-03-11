#ifndef QQSERVER_H
#define QQSERVER_H

#include <QObject>
#include <QHash>
#include <QReadWriteLock>


class Server : public QObject {
    Q_OBJECT

public:
    explicit Server(QObject * parent = nullptr);
    bool start(int port);
    void stop();
    ~Server();

public:
    QReadWriteLock lock;
    QHash<QString, class ClientHandler*> clientsMap;

private slots:
    void onNewConnection();

private:
    class QTcpServer * tcpServer;
};

// 1942794516
// 1423347801

#endif // QQSERVER_H
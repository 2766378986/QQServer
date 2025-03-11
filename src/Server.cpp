#include <Server.h>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <ClientHandler.h>

Server::Server(QObject * parent) : QObject(parent) {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

bool Server::start(int port) {
    if (!tcpServer->listen(QHostAddress::AnyIPv4, port)) {
        qDebug() << "Server failed to start:" << tcpServer->errorString();
        return false;
    }
    
    // 添加监听状态检查
    if (tcpServer->isListening()) {
        qDebug() << "Server is listening on:"
                 << tcpServer->serverAddress().toString()  // 输出监听的IP
                 << "port:" << tcpServer->serverPort();    // 输出监听的端口
    } else {
        qDebug() << "Server is not listening despite successful listen call";
    }
    return true;
}


void Server::onNewConnection() {
    QTcpSocket * socket = tcpServer->nextPendingConnection();
    ClientHandler * client = new ClientHandler(socket, this);
    client->start();
    qDebug() << "Client connected:" << socket->peerAddress().toString();
}

void Server::stop() {
    tcpServer->close();
    qDebug() << "Server stopped";
}

Server::~Server() {
}
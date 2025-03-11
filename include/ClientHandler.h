#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QReadWriteLock>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QQueue>

class Server;

class ClientHandler : public QThread {
    Q_OBJECT
public:
    ClientHandler(QTcpSocket * socket, Server * srv);
    ~ClientHandler();
    void run() override; // 启用事件循环
    bool databaseConnect(); // 连接数据库
    void addClient(const QString & account, ClientHandler * client); // 将登陆成功的用户添加到哈希存储
    void removeClient(const QString & account); // 当用户断开连接时，从哈希存储中移除
    ClientHandler * getClient(const QString & account); // 获取用户对应的客户端

public slots:
    void onReadyRead(); // 读取用户数据
    void onDisconnected(); // 当用户断开连接时，处理断开连接的事件
    void receiveMessage(const QJsonObject & json); // 收到别的客户端发送的消息，转发

public:
    QString pximapToBase64(const QPixmap & pixmap); // 将QPixmap转换为base64
    QPixmap base64ToPixmap(const QString & base64); // 将base64转换为QPixmap

    void dealLogin(const QJsonObject & json); // 处理登陆请求
    void dealRegister(const QJsonObject & json); // 处理注册请求

    void dealAvator(const QString & avator); // 上传头像
    void dealAskforAvator(const QString & account); // 请求头像

    void dealFindPassword1(const QString & account); // 找回密码第一步,查看是否有这个账号
    void dealFindPassword2(const QJsonObject & json); // 找回密码第二步，查看密保问题回答对了没有
    void dealFindPassword3(const QJsonObject & json); // 找回密码第三步
    
    void dealLoginFirst(const QJsonObject & json); // 处理用户登录成功后初始化
    
    void dealDeleteFriend(const QJsonObject & json); // 处理删除好友请求

    void dealSearchAccount(const QJsonObject & json); // 处理搜索账号请求

    void dealChangeInformation(const QJsonObject & json); // 处理用户更新个人信息
    void dealChangePassword(const QJsonObject & json); // 处理用户更新密码
    void dealChangePassword2(const QJsonObject & json); // 处理用户更新密码

    void dealLogout(const QJsonObject & json); // 处理用户登出

    void dealAddFriend(const QJsonObject & json); // 发送添加好友申请的功能
    void dealAddNewFriends(const QJsonObject & json); // 回应是否添加好友

    void dealMessages(const QJsonObject & json); // 处理用户发送的消息
    void sendNextMessage(); // 从队列发送下一条消息

    void dealAskDocument(const QJsonObject & json); // 处理用户要下载文件的要求

public: // 接受到别的服务器发送的消息，然后转发
    void forwardAddFriendRequest(const QJsonObject & json); // 向在线用户转发用户好友申请
    void forwardRequestPass(const QJsonObject & json); // 向在线用户更新他的好友列表
    void forwardBeDeleted(const QJsonObject & json); // 向在线用户转发自己被删除了
    void forwardKickedOffline(const QJsonObject & json); // 向在线用户转发自己被挤下线了
    void forwardMessages(const QJsonObject & json); // 向用户发送聊天信息
signals:
    void sendMessage(const QJsonObject & json); // 给背的客户端发送消息

private:
    static QMutex dbMutex; // 数据库互斥锁
    static QReadWriteLock lock; // 读写锁
    QMutex mutex; // 互斥锁
    QSqlDatabase db; // 数据库
    QTcpSocket * socket; 
    QJsonDocument jsonDoc;
    QJsonObject jsonObj;
    QByteArray jsonData;
    QByteArray bufferedData;
    Server * srv;
    QString randomNumber; // 注册随机生成账号
    QString account = "0"; // 当前账号
    QQueue<QJsonObject> messageQueue; // 存储待发送的消息
    bool isSending = false; // 是否正在发送
    int flag = 0; // 标志位
};

class DocumentWorker : public QObject {
    Q_OBJECT

public:
    DocumentWorker(const QString & filename, const QString & timestamp, QSqlDatabase db) :
     filename(filename), timestamp(timestamp), db(db) {}

signals:
    void resultReady(const QJsonObject & json);

public slots:
    void process(); // 处理查询文件

private:
    QString filename;
    QString timestamp;
    QSqlDatabase db;

};
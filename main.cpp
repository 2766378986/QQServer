#include <QCoreApplication>
#include <Server.h>
#include <QSqlDatabase>
#include <QSqlQuery>

#define TEST_SERVER 1


int main(int argc, char * argv[]) {
    QCoreApplication a(argc, argv);

    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("server.db");
    db.open();

    QSqlQuery query;
    query.exec("SELECT qq_number, password FROM Users");
    while (query.next()) {
        QString qq_number = query.value(0).toString();
        QString password = query.value(1).toString();
        qDebug() << QString("账号密码为: (%1, %2)").arg(qq_number).arg(password);
    }


    Server server;
    server.start(57777);

    

    return a.exec();
}

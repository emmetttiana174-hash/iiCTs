#include "userdb.h"

#include <QSqlQuery>
#include <QDebug>
#include <QDir>

QSqlDatabase UserDB::db;

bool UserDB::init() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbFile = QDir::homePath()
            + QDir::separator()
            + QLatin1String(CTISS_USERS_DB_FILENAME);
    db.setDatabaseName(dbFile);
    if (!db.open()) return false;
    QSqlQuery query;
    return query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT)");
}

bool UserDB::validate(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username=? AND password=?");
    query.addBindValue(username);
    query.addBindValue(password);
    query.exec();
    return query.next();
}

bool UserDB::registerUser(const QString &username, const QString &password) {


    QStringList allowedUsers = { "guoyi", "yiguo", "gy", "admin", "wyk" , "lyl", "wangyankun", "liyule"};

    if (!allowedUsers.contains(username)) {
        qDebug() << "Username is not in the allowed registration list:" << username;
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    return query.exec();
}


bool UserDB::updatePassword(const QString &username, const QString &oldPassword, const QString &newPassword, const QString &valFlag)
{
    bool needValidate = (valFlag.toLower() != "false");

    if (needValidate && !validate(username, oldPassword)) {
        qDebug() << "Old password verification failed";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE users SET password = ? WHERE username = ?");
    query.addBindValue(newPassword);
    query.addBindValue(username);
    return query.exec();
}




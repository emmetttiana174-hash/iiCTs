#ifndef USERDB_H
#define USERDB_H

#include <QSqlDatabase>
#include <QString>

class UserDB {
public:
    static bool init();
    static bool validate(const QString &username, const QString &password);
    static bool registerUser(const QString &username, const QString &password);
    static bool updatePassword(const QString &username, const QString &oldPassword, const QString &newPassword, const QString &valFlag = "true");

private:
    static QSqlDatabase db;
};

#endif // USERDB_H

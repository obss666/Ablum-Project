#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>

class DatabaseManager {
public:
    static bool initDatabase();
    static bool setSettingValue(const QString &fileName, const QString &filePath);
    static QList<QString> getAllFile();
private:
    DatabaseManager() = delete;
    static QSqlDatabase getDatabase();
};

#endif // DATABASEMANAGER_H

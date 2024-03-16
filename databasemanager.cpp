#include "DatabaseManager.h"

bool DatabaseManager::initDatabase() {
    QSqlDatabase db = getDatabase();
    QSqlQuery query;
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Error opening database: " << db.lastError().text();
            return false;
        }

        if (!query.exec("CREATE TABLE IF NOT EXISTS file_tree ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "file_name TEXT, "
                        "file_path TEXT)")) {
            qDebug() << "Error creating table: " << query.lastError().text();
            return false;
        }

    }
    return true;
}

bool DatabaseManager::setSettingValue(const QString &fileName, const QString &filePath)
{
    QSqlDatabase db = getDatabase();
    QSqlQuery query;
    query.prepare("INSERT INTO file_tree (file_name, file_path) VALUES (:file_name, :file_path)");
    query.bindValue(":file_name", fileName);
    query.bindValue(":file_path", filePath);
    return query.exec();
}


QList<QString> DatabaseManager::getAllFile() {
    QList<QString> fileList;

    QSqlDatabase db = getDatabase();

    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return fileList;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT file_path FROM file_tree")) {
        qDebug() << "Failed to execute query: " << query.lastError().text();
        return fileList;
    }

    while (query.next()) {
        QString filePath = query.value(0).toString();
        fileList.append(filePath);
    }

    return fileList;
}

QSqlDatabase DatabaseManager::getDatabase() {
    static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("settings.db");
    return db;
}

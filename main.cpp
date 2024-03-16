#include "mainwindow.h"
#include "DatabaseManager.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qss(":/style/style.qss");

    if(qss.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    } else {
        return 0;
    }

    MainWindow w;
    w.setWindowTitle("Album");
    w.show();

    return a.exec();
}

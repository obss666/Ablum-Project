#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;
    QWidget *_protree;  //左边目录树
    QWidget *_picshow;  //右边图片显示

private slots:
    void SlotCreatePro(bool); // 创建项目槽函数
    void SlotOpenPro(bool);   // 打开项目槽函数

signals:
    void SigOpenPro(const QString &path);
};
#endif // MAINWINDOW_H

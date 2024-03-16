#ifndef PROTREETHREAD_H
#define PROTREETHREAD_H

#include <QThread>
#include <QTreeWidget>

class ProTreeThread : public QThread
{
    Q_OBJECT

public:
    ProTreeThread(const QString &src_path, const QString &dist_path, QTreeWidgetItem *parent_item,
                  int file_count, QTreeWidget *self, QTreeWidgetItem *root, QObject *parent = nullptr);
    ~ProTreeThread();

protected:
    virtual void run();

private:
    void CreateProTree(const QString &src_path, const QString &dist_path, QTreeWidgetItem *parent_item,
                       int &file_count, QTreeWidget *self, QTreeWidgetItem *root, QTreeWidgetItem *preItem = nullptr);

    QString _src_path;   // 源地址
    QString _dist_path;  // 目的地址
    int _file_count;     // 文件数量
    QTreeWidgetItem *_parent_item;
    QTreeWidget *_self;
    QTreeWidgetItem *_root;
    bool _bstop; //是否停止

public slots:
    void SlotCancelProgress();

signals:
    void SigUpdateProgress(int);
    void SigFinishProgress(int);

};

#endif // PROTREETHREAD_H

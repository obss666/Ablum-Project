#ifndef OPENTREETHREAD_H
#define OPENTREETHREAD_H

#include <QThread>
#include <QTreeWidget>

class OpenTreeThread : public QThread
{
    Q_OBJECT

public:
    explicit OpenTreeThread(const QString &src_path, int &file_count, QTreeWidget *self, QObject *parent = nullptr);
    ~OpenTreeThread();

protected:
    virtual void run();

private:
    void RecursiveProTree(const QString &src_path, int &file_count, QTreeWidget *self,
                          QTreeWidgetItem *root, QTreeWidgetItem *parent_item, QTreeWidgetItem *preitem);
    QString _src_path;
    int _file_count;
    QTreeWidget *_self;
    QTreeWidgetItem *_root;
    bool _bstop;

signals:
    void SigFinishProgress(int);
    void SigUpdateProgress(int);

public slots:
    void SlotCancelProgress();
};

#endif // OPENTREETHREAD_H

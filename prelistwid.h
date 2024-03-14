#ifndef PRELISTWID_H
#define PRELISTWID_H
#include <QTreeWidgetItem>
#include <QListWidget>

class PreListWid : public QListWidget
{
    Q_OBJECT
public:
    PreListWid(QWidget *parent = nullptr);
    virtual ~PreListWid();

private:
    QMap<QString, QListWidgetItem*> _set_items;
    void AddListItem(const QString &path);
    int _global;
    QPoint _pos_origin;
    int _last_index;

public slots:
    void SlotUpPreList(QTreeWidgetItem *tree_item);
    void SlotUpSelect(QTreeWidgetItem *tree_item);
    void SlotItemPressed(QListWidgetItem *item);

signals:
    void SigUpSelectShow(QString &path);
};

#endif // PRELISTWID_H

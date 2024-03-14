#ifndef PICANIMATIONWID_H
#define PICANIMATIONWID_H
#include <QWidget>
#include <QTreeWidgetItem>

class PicAnimationWid : public QWidget
{
    Q_OBJECT
public:
    explicit PicAnimationWid(QWidget *parent = nullptr);
    ~PicAnimationWid();
    void SetPixmap(QTreeWidgetItem *item);
    void Start();
    void Stop();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    float _factor;
    QTimer *_timer;
    QPixmap _pixmap1;
    QPixmap _pixmap2;
    QTreeWidgetItem *_cur_item;
    QMap<QString, QTreeWidgetItem*> _map_items;
    bool _b_start;

signals:

private slots:
    void TimeOut();

};

#endif // PICANIMATIONWID_H

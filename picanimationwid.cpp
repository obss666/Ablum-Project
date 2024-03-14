#include "picanimationwid.h"
#include <QTimer>
#include "protreeitem.h"
#include <QPainter>

PicAnimationWid::PicAnimationWid(QWidget *parent)
    : QWidget{parent}, _factor(0.0), _cur_item(nullptr), _b_start(false)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &PicAnimationWid::TimeOut);
}

PicAnimationWid::~PicAnimationWid()
{

}

void PicAnimationWid::SetPixmap(QTreeWidgetItem *item)
{
    if(!item){
        return;
    }

    auto * tree_item = dynamic_cast<ProTreeItem*>(item);
    auto path = tree_item->GetPath();
    _pixmap1.load(path);
    _cur_item = tree_item;
    if(_map_items.find(path) == _map_items.end()){
        _map_items[path]=tree_item;
        // qDebug() << "SetPixmap path is " << path << Qt::endl;
        // emit SigUpPreList(item);
    }

    // emit SigSelectItem(item);

    auto * next_item = tree_item->GetNextItem();
    if(!next_item){
        return;
    }

    auto next_path = next_item->GetPath();
    _pixmap2.load(next_path);
    if(_map_items.find(next_path) == _map_items.end()){
        _map_items[next_path] = next_item;
        // emit SigUpPreList(next_item);
    }
}

void PicAnimationWid::Start()
{
    _factor = 0;
    _timer->start(25);
    _b_start = true;
}

void PicAnimationWid::Stop()
{
    _timer->stop();
    _factor = 0;
    _b_start = false;
}

void PicAnimationWid::paintEvent(QPaintEvent *event)
{
    if(_pixmap1.isNull()){
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = geometry();

    int w = rect.width();
    int h = rect.height();
    _pixmap1 = _pixmap1.scaled(w, h, Qt::KeepAspectRatio);
    int alpha = 255 * (1.0f - _factor);

    QPixmap alphaPixmap(_pixmap1.size());
    alphaPixmap.fill(Qt::transparent);

    QPainter p1(&alphaPixmap);
    p1.setCompositionMode(QPainter::CompositionMode_Source);
    p1.drawPixmap(0, 0, _pixmap1);
    p1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p1.fillRect(alphaPixmap.rect(), QColor(0, 0, 0, alpha));
    p1.end();

    int x = (w - _pixmap1.width()) / 2;
    int y = (h - _pixmap1.height()) / 2;
    painter.drawPixmap(x, y, alphaPixmap);

    if(_pixmap2.isNull()){
        return;
    }

    _pixmap2 = _pixmap2.scaled(w, h, Qt::KeepAspectRatio);
    alpha = 255 * (_factor);
    QPixmap alphaPixmap2(_pixmap2.size());
    alphaPixmap2.fill(Qt::transparent);
    QPainter p2(&alphaPixmap2);
    p2.setCompositionMode(QPainter::CompositionMode_Source);
    p2.drawPixmap(0, 0, _pixmap2);
    p2.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p2.fillRect(alphaPixmap2.rect(), QColor(0, 0, 0, alpha));
    p2.end();
    x = (w - _pixmap2.width()) / 2;
    y = (h - _pixmap2.height()) / 2;
    painter.drawPixmap(x, y, alphaPixmap2);
}

void PicAnimationWid::TimeOut()
{
    if(!_cur_item){
        Stop();
        update();
        return;
    }

    _factor = _factor + 0.01;

    if(_factor >= 1){
        _factor = 0;
        auto * cur_pro_item = dynamic_cast<ProTreeItem*>(_cur_item);
        auto * next_pro_item = cur_pro_item->GetNextItem();
        if(!next_pro_item){
            Stop();
            update();
            return;
        }
        SetPixmap(next_pro_item);
        update();
        return;
    }
    update();
}

#include "prelistwid.h"
#include <protreeitem.h>
#include "const.h"
#include <QPainter>
#include "prelistitem.h"
#include <QGuiApplication>

PreListWid::PreListWid(QWidget *parent) : QListWidget(parent), _global(0), _last_index(15) {
    this->setViewMode(QListWidget::IconMode);
    this->setIconSize(QSize(PREICON_SIZE, PREICON_SIZE));
    this->setSpacing(5);
    connect(this, &PreListWid::itemPressed, this, &PreListWid::SlotItemPressed);
}

PreListWid::~PreListWid()
{

}

void PreListWid::AddListItem(const QString &path)
{
    QPixmap src_pixmap(path);
    src_pixmap = src_pixmap.scaled(PREICON_SIZE,PREICON_SIZE,Qt::KeepAspectRatio);
    QPixmap dst_pixmap(QSize(PREICON_SIZE, PREICON_SIZE));
    auto src_width = src_pixmap.width();
    auto src_height = src_pixmap.height();
    auto dist_width = dst_pixmap.width();
    auto dist_height = dst_pixmap.height();
    dst_pixmap.fill(QColor(220,220,220, 50));
    QPainter painter(&dst_pixmap);
    auto x = (dist_width-src_width)/2;
    auto y = (dist_height-src_height)/2;
    painter.drawPixmap(x,y,src_pixmap);
    _global++;
    PreListItem *pItem = new PreListItem(QIcon(dst_pixmap),path,_global,this);
    pItem->setSizeHint(QSize(PREITEM_SIZE,PREITEM_SIZE));
    this->addItem(pItem); // 显示图片
    _set_items[path] = pItem;
    if(_global == 1){
        _pos_origin = this->pos();
    }
}


void PreListWid::SlotUpPreList(QTreeWidgetItem *tree_item)
{
    if(!tree_item){
        //qDebug() << "tree_item is empty" << Qt::endl;
        return;
    }
    auto * pro_item = dynamic_cast<ProTreeItem*>(tree_item);
    auto path = pro_item->GetPath();
    auto iter = _set_items.find(path);
    if(iter != _set_items.end()){
        // qDebug() << "path " <<path<< " exists" << Qt::endl;
        return;
    }
    AddListItem(path);
}

void PreListWid::SlotUpSelect(QTreeWidgetItem *tree_item) // 显示当前选中项
{
    if(!tree_item){
        qDebug() << "tree_item is empty" << Qt::endl;
        return;
    }
    auto * pro_item = dynamic_cast<ProTreeItem*>(tree_item);
    auto path = pro_item->GetPath();
    auto iter = _set_items.find(path);
    if(iter == _set_items.end()){
        // qDebug() << "path " <<path<< " not exists" << endl;
        return;
    }
    auto * list_item = dynamic_cast<PreListItem*>(iter.value());
    auto index = list_item->GetIndex();

    if(index > 15){
        auto pos_cur = this->pos();
        this->move(pos_cur.x() - (index - _last_index) * 100, pos_cur.y());
        _last_index = index;
    }else{
        this->move(_pos_origin);
        _last_index = 15;
    }
    this->setCurrentItem(iter.value());
}

void PreListWid::SlotItemPressed(QListWidgetItem *item)
{
    if(QGuiApplication::mouseButtons() != Qt::LeftButton){
        return;
    }
    auto *list_item = dynamic_cast<PreListItem*>(item);
    auto path = list_item->GetPath();
    this->setCurrentItem(item);
    emit SigUpSelectShow(path);
}

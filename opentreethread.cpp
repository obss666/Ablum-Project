#include "opentreethread.h"
#include <Qdir>
#include <QDebug>
#include "protreeitem.h"
#include "databasemanager.h"
#include "const.h"

OpenTreeThread::OpenTreeThread(const QString &src_path, int &file_count, QTreeWidget *self, QObject *parent) :
    QThread (parent), _src_path(src_path), _file_count(file_count), _self(self), _root(nullptr), _bstop(false)
{

}

OpenTreeThread::~OpenTreeThread()
{

}

void OpenTreeThread::run()
{
    //创建根节点
    QDir src_dir(_src_path);
    auto name = src_dir.dirName();
    auto * item = new ProTreeItem(_self, name, _src_path, TreeItemPro); // 根item
    item->setData(0,Qt::DisplayRole, name);
    item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
    item->setData(0,Qt::ToolTipRole, _src_path);
    _root = item;
    //读取根节点下目录和文件
    RecursiveProTree(_src_path, _file_count, _self, _root, item, nullptr);


    if(_bstop){
        auto path = dynamic_cast<ProTreeItem*>(_root)->GetPath();
        auto index = _self->indexOfTopLevelItem(_root);
        delete _self->takeTopLevelItem(index);
        QDir dir(path);
        dir.removeRecursively();
        return;
    }
    emit SigFinishProgress(_file_count);
}

void OpenTreeThread::RecursiveProTree(const QString &src_path, int &file_count, QTreeWidget *self,
                                      QTreeWidgetItem *root, QTreeWidgetItem *parent_item, QTreeWidgetItem *preitem)
{
    QDir src_dir(src_path); //设置文件过滤器
    src_dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);//除了目录或文件，其他的过滤掉
    src_dir.setSorting(QDir::Name);//优先显示名字
    QFileInfoList list = src_dir.entryInfoList();

    QFileInfoList dirList, fileList;
    for (const auto &fileInfo : list) {
        if (fileInfo.isDir()) {
            dirList.append(fileInfo);
        } else {
            fileList.append(fileInfo);
        }
    }

    // 先处理目录
    for (const auto &fileInfo : dirList) {
        if(_bstop){
            return;
        }
        file_count++;
        emit SigUpdateProgress(file_count);
        auto * item = new ProTreeItem(parent_item, fileInfo.fileName(), fileInfo.absoluteFilePath(), root, TreeItemDir);
        item->setData(0,Qt::DisplayRole, fileInfo.fileName());
        item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
        item->setData(0,Qt::ToolTipRole, fileInfo.absoluteFilePath());
        RecursiveProTree(fileInfo.absoluteFilePath(), file_count, self, root, item, nullptr);
    }
    // 处理文件
    for (const auto &fileInfo : fileList) {
        if(_bstop){
            return;
        }
        const QString &suffix = fileInfo.completeSuffix();
        if(suffix != "png" && suffix != "jpeg" && suffix != "jpg"){
            continue;
        }
        file_count++;
        emit SigUpdateProgress(file_count);
        auto * item = new ProTreeItem(parent_item, fileInfo.fileName(), fileInfo.absoluteFilePath(), root, TreeItemPic);
        item->setData(0,Qt::DisplayRole, fileInfo.fileName());
        item->setData(0,Qt::DecorationRole, QIcon(":/icon/pic.png"));
        item->setData(0,Qt::ToolTipRole, fileInfo.absoluteFilePath());
        if(preitem){
            auto* pre_proitem = dynamic_cast<ProTreeItem*>(preitem);
            pre_proitem->SetNextItem(item);
        }
        item->SetPreItem(preitem);
        preitem = item;
    }
    // parent_item->setExpanded(true);
    emit SigFinishProgress(file_count);
}

void OpenTreeThread::SlotCancelProgress()
{
    this->_bstop = true;
}

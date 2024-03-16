#include "protreethread.h"
#include <QDir>
#include "protreeitem.h"
#include "const.h"

ProTreeThread::ProTreeThread(const QString &src_path, const QString &dist_path, QTreeWidgetItem *parent_item,
                             int file_count, QTreeWidget *self, QTreeWidgetItem *root, QObject *parent) :
    QThread (parent),_src_path(src_path),_dist_path(dist_path),
      _file_count(file_count),_parent_item(parent_item),_self(self),_root(root),_bstop(false)
{

}

ProTreeThread::~ProTreeThread()
{

}

void ProTreeThread::run()
{
    CreateProTree(_src_path, _dist_path, _parent_item, _file_count, _self, _root);
    if(_bstop){ // 点击取消
        auto path = dynamic_cast<ProTreeItem*>(_root)->GetPath();
        auto index = _self->indexOfTopLevelItem(_root);
        delete _self->takeTopLevelItem(index);
        QDir dir(path);
        dir.removeRecursively();
        return;
    }
    emit SigFinishProgress(_file_count);
}

void ProTreeThread::CreateProTree(const QString &src_path, const QString &dist_path, QTreeWidgetItem *parent_item,
                                  int &file_count, QTreeWidget *self, QTreeWidgetItem *root, QTreeWidgetItem* preItem)
{
    if(_bstop){
        return;
    }
    bool needcopy = true;
    if(src_path == dist_path){
        needcopy = false;
    }
    QDir import_dir(src_path);
    import_dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot); //除了目录或文件，其他的过滤掉
    import_dir.setSorting(QDir::Name); //优先显示名字

    QFileInfoList dirList, fileList;
    QFileInfoList list = import_dir.entryInfoList();
    for (const auto &fileInfo : list) {
        if (fileInfo.isDir()) {
            dirList.append(fileInfo);
        } else {
            fileList.append(fileInfo);
        }
    }

    // 处理目录
    for (const auto &fileInfo : dirList) {
        if(_bstop){
            return;
        }
        file_count++;
        emit SigUpdateProgress(file_count);
        QDir dist_dir(dist_path);
        QString sub_dist_path = dist_dir.absoluteFilePath(fileInfo.fileName());
        QDir sub_dist_dir(sub_dist_path);
        if(!sub_dist_dir.exists()){ //不能存在则创建
            bool ok = sub_dist_dir.mkpath(sub_dist_path);
            if(!ok){
                continue;
            }
        }
        auto * item = new ProTreeItem(parent_item, fileInfo.fileName(), sub_dist_path, root, TreeItemDir);
        item->setData(0,Qt::DisplayRole, fileInfo.fileName());
        item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
        item->setData(0,Qt::ToolTipRole, sub_dist_path);
        CreateProTree(fileInfo.absoluteFilePath(), sub_dist_path, item, file_count, self, root, nullptr);
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
        if(!needcopy){
            continue;
        }
        QDir dist_dir(dist_path);
        QString dist_file_path = dist_dir.absoluteFilePath(fileInfo.fileName());
        if(!QFile::copy(fileInfo.absoluteFilePath(), dist_file_path)) {
            continue;
        }
        auto * item = new ProTreeItem(parent_item, fileInfo.fileName(), dist_file_path, root, TreeItemPic);
        item->setData(0,Qt::DisplayRole, fileInfo.fileName());
        item->setData(0,Qt::DecorationRole, QIcon(":/icon/pic.png"));
        item->setData(0,Qt::ToolTipRole, dist_file_path);
        if(preItem){
            auto* pre_proitem = dynamic_cast<ProTreeItem*>(preItem);
            pre_proitem->SetNextItem(item);
        }
        item->SetPreItem(preItem);
        preItem = item;
    }
    // parent_item->setExpanded(true);
}

void ProTreeThread::SlotCancelProgress()
{
    this->_bstop = true;
}

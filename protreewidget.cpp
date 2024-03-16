#include "protreewidget.h"
#include <QDir>
#include "protreeitem.h"
#include "const.h"
#include "qheaderview.h"
#include <QGuiApplication>
#include <QMenu>
#include <QFileDialog>
#include "protreethread.h"
#include "removeprodialog.h"
#include "opentreethread.h"
#include "slideshowdlg.h"
#include "databasemanager.h"

ProTreeWidget::ProTreeWidget(QWidget *parent): QTreeWidget(parent), _right_btn_item(nullptr), _active_item(nullptr),
    _selected_item(nullptr), _dialog_progress(nullptr), _open_progressdlg(nullptr),
    _thread_create_pro(nullptr), _thread_open_pro(nullptr), _slide_show_dlg(nullptr)
{
    this->header()->hide();

    _action_import = new QAction(QIcon(":/icon/import.png"),tr("导入文件"), this);
    _action_setstart = new QAction(QIcon(":/icon/core.png"),tr("设置活动项目"), this);
    _action_closepro = new QAction(QIcon(":/icon/close.png"),tr("关闭项目"), this);
    _action_slidshow = new QAction(QIcon(":/icon/slideshow.png"),tr("轮播图播放"), this);

    connect(this, &ProTreeWidget::itemPressed, this, &ProTreeWidget::SlotItemPressed); //右键点击触发菜单
    connect(_action_import, &QAction::triggered, this, &ProTreeWidget::SlotImport);
    connect(_action_setstart, &QAction::triggered, this, &ProTreeWidget::SlotSetActive);
    connect(_action_closepro, &QAction::triggered, this, &ProTreeWidget::SlotClosePro);
    connect(_action_slidshow, &QAction::triggered, this, &ProTreeWidget::SlotSlideShow);

    connect(this, &ProTreeWidget::itemDoubleClicked, this, &ProTreeWidget::SlotDoubleClickItem); //当前的根目录中的item被双击
}

void ProTreeWidget::OpenPro(const QString& path)
{
    _set_path.insert(path);
    int file_count = 0;
    _thread_open_pro = std::make_shared<OpenTreeThread>(path, file_count, this, nullptr);
    _thread_open_pro->start();
}

void ProTreeWidget::AddProToTree(const QString &name, const QString &path)
{
    QDir dir(path);

    QString file_path = dir.absoluteFilePath(name);

    //检测重名，判断路径和名字都一样则拒绝加入
    if(_set_path.find(file_path) != _set_path.end()) {
        return;
    }
    //构造项目用的文件夹
    QDir pro_dir(file_path);
    //如果文件夹不存在则创建
    if(!pro_dir.exists()){
        bool enable = pro_dir.mkpath(file_path);
        if(!enable){
            qDebug() << "pro_dir make path failed" << Qt::endl;
            return;
        }
    }
    _set_path.insert(file_path);

    auto *item = new ProTreeItem(this, name, file_path, TreeItemPro);
    item->setData(0, Qt::DisplayRole, name);
    item->setData(0, Qt::DecorationRole, QIcon(":/icon/dir.png"));
    item->setData(0, Qt::ToolTipRole, file_path);
    this->addTopLevelItem(item); //添加到目录树

    DatabaseManager::setSettingValue(name, file_path);
}

void ProTreeWidget::SlotItemPressed(QTreeWidgetItem *item, int column)
{
    if(QGuiApplication::mouseButtons() == Qt::RightButton)   //判断是否为右键
    {
        QMenu menu(this);
        int itemtype = (int)(item->type());
        if (itemtype == TreeItemPro) //如果是根目录
        {
            _right_btn_item = item;  //标记该项目被右键选中
            menu.addAction(_action_import);
            menu.addAction(_action_closepro);
            menu.addAction(_action_setstart);
            menu.addAction(_action_slidshow);
            menu.exec(QCursor::pos());   //菜单弹出位置为鼠标点击位置
        }
    }
}

void ProTreeWidget::SlotDoubleClickItem(QTreeWidgetItem *doubleItem, int column)
{
    if(QGuiApplication::mouseButtons() == Qt::LeftButton)   //判断是否为左键
    {
        auto * tree_doubleItem = dynamic_cast<ProTreeItem*>(doubleItem);
        if(!tree_doubleItem){
            return;
        }
        int itemtype = (int)(tree_doubleItem->type());
        if(itemtype == TreeItemPic){
            emit SigUpdateSelected(tree_doubleItem->GetPath());
            _selected_item = doubleItem;  //更新双击选中item
        }
    }
}

void ProTreeWidget::SlotImport() // 导入
{
    QFileDialog file_dialog; //文件对话框
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setWindowTitle(tr("选择导入的文件夹"));
    QString path = "";
    if(!_right_btn_item){
        path = QDir::currentPath();
        return ;
    }

    path = dynamic_cast<ProTreeItem*>(_right_btn_item)->GetPath(); //当前图片path
    file_dialog.setDirectory(path);
    file_dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (file_dialog.exec()){
        fileNames = file_dialog.selectedFiles(); // 获取选中文件
    }
    if(fileNames.length() <= 0){
        return;
    }
    QString import_path = fileNames.at(0);
    int file_count = 0;

    //导入文件线程
    _thread_create_pro = std::make_shared<ProTreeThread>(std::ref(import_path), std::ref(path), _right_btn_item,
                                                         file_count, this, _right_btn_item, nullptr);

    connect(_thread_create_pro.get(), &ProTreeThread::SigUpdateProgress, this, &ProTreeWidget::SlotUptateProgress);
    connect(_thread_create_pro.get(), &ProTreeThread::SigFinishProgress, this, &ProTreeWidget::SlotFinishProgress);

    _dialog_progress = new QProgressDialog(this);
    connect(_dialog_progress, &QProgressDialog::canceled, this, &ProTreeWidget::SlotCancelProgress);
    connect(this, &ProTreeWidget::SigCancelProgress, _thread_create_pro.get(), &ProTreeThread::SlotCancelProgress);

    _thread_create_pro->start();
    _dialog_progress->setWindowTitle("请等待文件载入。。。");
    _dialog_progress->setFixedWidth(PROGRESS_WIDTH);
    _dialog_progress->setRange(0, PROGRESS_WIDTH);
    _dialog_progress->exec();
}

void ProTreeWidget::SlotSetActive()
{
    if(!_right_btn_item) {
        return;
    }

    QFont nullFont;
    nullFont.setBold(false);

    if(_active_item) {
        _active_item->setFont(0, nullFont);
    }
    _active_item = _right_btn_item;
    nullFont.setBold(true);
    _active_item->setFont(0, nullFont);
}

void ProTreeWidget::SlotClosePro()
{
    RemoveProDialog remove_pro_dialog;
    if(remove_pro_dialog.exec() != QDialog::Accepted) return;

    bool b_remove = remove_pro_dialog.IsRemoved();
    auto index_right_btn = this->indexOfTopLevelItem(_right_btn_item);
    auto *protreeitem = dynamic_cast<ProTreeItem*>(_right_btn_item);
    auto *selecteditem = dynamic_cast<ProTreeItem*>(_selected_item);
    auto delete_path = protreeitem->GetPath();
    // qDebug() << "remove project from path: " << delete_path;
    _set_path.remove(delete_path);
    if(b_remove){
        QDir delete_dir(delete_path);
        delete_dir.removeRecursively();
    }
    if(protreeitem == _active_item){
        _active_item = nullptr;
    }
    if(selecteditem && protreeitem == selecteditem->GetRoot()){
        selecteditem = nullptr;
        _selected_item = nullptr;
        emit SigClearSelected();
    }
    delete this->takeTopLevelItem(index_right_btn);
    _right_btn_item = nullptr;

}

void ProTreeWidget::SlotUptateProgress(int count)
{
    if(!_dialog_progress){
        return;
    }
    _dialog_progress->setValue(count % PROGRESS_MAX);

}

void ProTreeWidget::SlotFinishProgress()
{
    if(!_dialog_progress){
        return;
    }
    _dialog_progress->setValue(PROGRESS_MAX);
    _dialog_progress->deleteLater();
}

void ProTreeWidget::SlotCancelProgress()
{
    emit SigCancelProgress();
    _dialog_progress->deleteLater();
}

void ProTreeWidget::SlotUptateOpenProgress(int count)
{
    if(!_open_progressdlg){
        return;
    }
    _open_progressdlg->setValue(count % PROGRESS_MAX);
}

void ProTreeWidget::SlotFinishOpenProgress()
{
    if(!_open_progressdlg){
        return;
    }
    _open_progressdlg->setValue(PROGRESS_MAX);
    _open_progressdlg->deleteLater();
}

void ProTreeWidget::SlotCancelOpenProgress()
{
    emit SigCancelOpenProgress();
    _open_progressdlg->deleteLater();
}

void ProTreeWidget::SlotSlideShow()
{
    if(!_right_btn_item) {
        return;
    }

    auto *right_pro_item = dynamic_cast<ProTreeItem*>(_right_btn_item);
    auto * last_child_item = right_pro_item->GetLastPicChild();
    if(!last_child_item){
        return;
    }

    qDebug()<< "last child item name is " << last_child_item->GetPath()<< Qt::endl;

    auto * first_child_item = right_pro_item->GetFirstPicChild();
    if(!first_child_item){
        return;
    }

    qDebug()<< "first child item name is " << first_child_item->GetPath()<< Qt::endl;

    _slide_show_dlg = std::make_shared<SlideShowDlg>(this, first_child_item, last_child_item);
    _slide_show_dlg->setModal(true);
    _slide_show_dlg->showMaximized();
}

void ProTreeWidget::SlotOpenPro(const QString& path)
{
    if(_set_path.find(path) != _set_path.end()){
        // qDebug() << "file has loaded" << Qt::endl;
        return;
    }
    _set_path.insert(path);
    int file_count = 0;
    QDir pro_dir(path);
    const QString& proname = pro_dir.dirName();
    _thread_open_pro = std::make_shared<OpenTreeThread>(path, file_count, this, nullptr);

    _open_progressdlg = new QProgressDialog(this);
    //连接更新进度框操作
    connect(_thread_open_pro.get(), &OpenTreeThread::SigUpdateProgress, this, &ProTreeWidget::SlotUptateOpenProgress);
    connect(_thread_open_pro.get(), &OpenTreeThread::SigFinishProgress, this, &ProTreeWidget::SlotFinishOpenProgress);
    connect(_open_progressdlg, &QProgressDialog::canceled, this, &ProTreeWidget::SlotCancelOpenProgress);
    connect(this, &ProTreeWidget::SigCancelOpenProgress, _thread_open_pro.get(), &OpenTreeThread::SlotCancelProgress);

    _thread_open_pro->start();
    _open_progressdlg->setWindowTitle("请等待文件载入。。。");
    _open_progressdlg->setFixedWidth(PROGRESS_WIDTH);
    _open_progressdlg->setRange(0, PROGRESS_WIDTH);
    _open_progressdlg->exec();
}

void ProTreeWidget::SlotNextShow()
{
    if(!_selected_item){
        return;
    }
    auto * curItem = dynamic_cast<ProTreeItem*>(_selected_item)->GetNextItem();
    if(!curItem){
        return;
    }
    emit SigUpdatePic(curItem->GetPath());
    _selected_item = curItem;
    this->setCurrentItem(curItem);
}

void ProTreeWidget::SlotPreShow()
{
    if(!_selected_item){
        return;
    }
    auto * curItem = dynamic_cast<ProTreeItem*>(_selected_item)->GetPreItem();
    if(!curItem){
        return;
    }
    emit SigUpdatePic(curItem->GetPath());
    _selected_item = curItem;
    this->setCurrentItem(curItem);
}


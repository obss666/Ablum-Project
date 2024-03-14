#ifndef PROTREEWIDGET_H
#define PROTREEWIDGET_H

#include <QTreeWidget>
#include <QAction>
#include <QProgressDialog>
#include "protreethread.h"
#include "opentreethread.h"
// #include <QtMultimedia/QMediaPlayer>

class SlideShowDlg;

class ProTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ProTreeWidget(QWidget *parent = nullptr);
    void AddProToTree(const QString &name, const QString &path);

private:
    QSet<QString> _set_path;             //图片集合
    QTreeWidgetItem *_right_btn_item;    //当前右击的item
    QTreeWidgetItem *_active_item;       //当前活动的item
    QTreeWidgetItem *_selected_item;    //当前选择的item
    QAction *_action_import;            //导入文件动作
    QAction *_action_setstart;          //设置活动item动作
    QAction *_action_closepro;          //关闭文件动作
    QAction *_action_slidshow;          //展示轮播图动作
    QProgressDialog *_dialog_progress;  //导入文件对话框
    QProgressDialog *_open_progressdlg; //打开文件对话框
    std::shared_ptr<ProTreeThread> _thread_create_pro;
    std::shared_ptr<OpenTreeThread> _thread_open_pro;
    std::shared_ptr<SlideShowDlg> _slide_show_dlg;  //轮播图对话框

public slots:
    void SlotOpenPro(const QString &path);
    void SlotNextShow();
    void SlotPreShow();

private slots:
    void SlotItemPressed(QTreeWidgetItem *item, int column);
    void SlotDoubleClickItem(QTreeWidgetItem *doubleItem, int column);
    void SlotImport();
    void SlotSetActive();
    void SlotClosePro();
    void SlotUptateProgress(int count);
    void SlotFinishProgress();
    void SlotCancelProgress();
    void SlotUptateOpenProgress(int count);
    void SlotFinishOpenProgress();
    void SlotCancelOpenProgress();
    void SlotSlideShow();

signals:
    void SigCancelProgress();
    void SigCancelOpenProgress();
    void SigUpdateSelected(const QString &);
    void SigUpdatePic(const QString &);
    void SigClearSelected();
};

#endif // PROTREEWIDGET_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "wizard.h"
#include "protree.h"
#include <QFileDialog>
#include "protreewidget.h"
#include "picshow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setMinimumSize(1400, 800);
    ui->setupUi(this);
    //创建项目菜单
    QMenu *menu_file = menuBar()->addMenu(tr("文件(&F)")); //添加到菜单栏
    //创建项目动作
    QAction *act_create_pro = new QAction(QIcon(":/icon/createpro.png"), tr("创建项目"), this);
    act_create_pro->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    menu_file->addAction(act_create_pro);
    //打开项目动作
    QAction *act_open_pro = new QAction(QIcon(":/icon/openpro.png"), tr("打开项目"), this);
    act_open_pro->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    menu_file->addAction(act_open_pro);

    //创建设置菜单
    QMenu *menu_set = menuBar()->addMenu(tr("设置(&S)"));  //添加到菜单栏
    //设置背景音乐
    QAction *act_music = new QAction(QIcon(":/icon/music.png"), tr("背景音乐"),this);
    act_music->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    menu_set->addAction(act_music);

    //连接创建项目槽函数
    connect(act_create_pro, &QAction::triggered, this, &MainWindow::SlotCreatePro);
    //连接打开项目的槽函数
    connect(act_open_pro, &QAction::triggered, this, &MainWindow::SlotOpenPro);

    _protree = new ProTree(); // dialog
    ui->proLayout->addWidget(_protree);
    QTreeWidget *tree_widget = dynamic_cast<ProTree*>(_protree)->GetTreeWidget();
    auto *pro_tree_widget = dynamic_cast<ProTreeWidget*>(tree_widget);
    //连接打开项目的槽函数
    connect(this, &MainWindow::SigOpenPro, pro_tree_widget, &ProTreeWidget::SlotOpenPro);

    _picshow = new PicShow(); // dialog
    ui->picLayout->addWidget(_picshow);
    auto * pro_pic_show = dynamic_cast<PicShow*>(_picshow);
    connect(pro_tree_widget, &ProTreeWidget::SigUpdateSelected,pro_pic_show, &PicShow::SlotSelectItem);
    connect(pro_pic_show, &PicShow::SigPreClicked, pro_tree_widget, &ProTreeWidget::SlotPreShow);
    connect(pro_pic_show, &PicShow::SigNextClicked, pro_tree_widget, &ProTreeWidget::SlotNextShow);
    connect(pro_tree_widget, &ProTreeWidget::SigUpdatePic, pro_pic_show, &PicShow::SlotUpdatePic);
    connect(pro_tree_widget, &ProTreeWidget::SigClearSelected, pro_pic_show, &PicShow::SlotDeleteItem);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    auto *pro_pic_show = dynamic_cast<PicShow*>(_picshow);
    pro_pic_show->ReloadPic();
    QMainWindow::resizeEvent(event);
}

void MainWindow::SlotCreatePro(bool)
{
    // qDebug() << "slot create pro triggered" << Qt::endl;
    Wizard wizard(this);  // 向导
    wizard.setWindowTitle(tr("创建项目"));
    auto *page = wizard.page(0);
    page->setTitle(tr("设置项目配置"));

    //连接槽函数，把项目配置传回来
    connect(&wizard, &Wizard::SigProSettings, dynamic_cast<ProTree*>(_protree), &ProTree::AddProToTree);
    wizard.show();
    wizard.exec();

    disconnect(&wizard);
}

void MainWindow::SlotOpenPro(bool)
{
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setWindowTitle("选择导入的文件夹");
    file_dialog.setDirectory(QDir::currentPath());
    file_dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (file_dialog.exec()){
        fileNames = file_dialog.selectedFiles();
    }
    if(fileNames.length() <= 0){
        return;
    }
    QString import_path = fileNames.at(0);
    emit SigOpenPro(import_path); // 发射pro已经打开的信号，用于连接ProTreeWidget
}

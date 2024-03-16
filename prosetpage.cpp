#include "prosetpage.h"
#include "ui_prosetpage.h"
#include <QLineEdit>
#include <QDir>
#include <QFileDialog>

ProSetPage::ProSetPage(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::ProSetPage)
{
    ui->setupUi(this);
    registerField("proPath", ui->lineEdit_2); //将文本框字段与lineEdit关联
    registerField("proName*", ui->lineEdit);

    connect(ui->lineEdit, &QLineEdit::textEdited, this, &ProSetPage::completeChanged);  // 编辑时判断是否有效
    connect(ui->lineEdit_2, &QLineEdit::textEdited, this, &ProSetPage::completeChanged);

    QString curPath = QDir::currentPath(); // 获取当前应用程序的工作目录。
    ui->lineEdit_2->setText(curPath);
    ui->lineEdit_2->setCursorPosition(ui->lineEdit_2->text().size()); //光标放到最后

    // 清除按钮
    ui->lineEdit->setClearButtonEnabled(true);
    ui->lineEdit_2->setClearButtonEnabled(true);
}

ProSetPage::~ProSetPage()
{
    delete ui;
}

void ProSetPage::GetProSettings(QString &name, QString &path)
{
    name = ui->lineEdit->text();
    path = ui->lineEdit_2->text();
}

bool ProSetPage::isComplete() const
{
    if(ui->lineEdit->text() == "" || ui->lineEdit_2->text() == ""){
        return false;
    }
    //判断路径是否合理
    QDir dir(ui->lineEdit_2->text());
    if(!dir.exists())
    {
        // qDebug()<<"file path is not exists" << Qt::endl;
        ui->tips->setText("项目路径不存在");
        return false;
    }

    //判断项目文件夹是否已经存在
    QString absFilePath = dir.absoluteFilePath(ui->lineEdit->text()); // 拼接完整路径

    // qDebug() << "absFilePath is " << absFilePath;

    QDir dist_dir(absFilePath);
    if(dist_dir.exists()){
        ui->tips->setText("项目已经存在, 请更改项目名称或路径！");
        return false;
    }

    ui->tips->setText("");
    return QWizardPage::isComplete();
}

void ProSetPage::on_pushButton_clicked()
{
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory); // 设置为打开目录
    file_dialog.setWindowTitle("请选择导入的文件夹");

    auto path = QDir::currentPath();
    file_dialog.setDirectory(path);

    file_dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (file_dialog.exec()){
        fileNames = file_dialog.selectedFiles();
    }
    if(fileNames.length() <= 0){
        return;
    }
    QString import_path = fileNames.at(0);
    // qDebug() << "import_path is " << import_path << Qt::endl;
    ui->lineEdit_2->setText(import_path);
}


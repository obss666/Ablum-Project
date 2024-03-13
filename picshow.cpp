#include "picshow.h"
#include "ui_picshow.h"
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>

PicShow::PicShow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PicShow)
{
    ui->setupUi(this);
    ui->previousBtn->SetIcons(":/icon/previous.png", ":/icon/previous_hover.png", ":/icon/previous_press.png");
    ui->nextBtn->SetIcons(":/icon/next.png", ":/icon/next_hover.png", ":/icon/next_press.png");
}

PicShow::~PicShow()
{
    delete ui;
}

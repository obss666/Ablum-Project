#ifndef PICSHOW_H
#define PICSHOW_H

#include <QDialog>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>

namespace Ui {
class PicShow;
}

class PicShow : public QDialog
{
    Q_OBJECT

public:
    explicit PicShow(QWidget *parent = nullptr);
    ~PicShow();

protected:
    bool event(QEvent *event) override;

private:
    Ui::PicShow *ui;
    QPropertyAnimation *_animation_show_pre;
    QPropertyAnimation *_animation_show_next;
    void ShowPreNextBtns(bool b_show);
    bool _b_btnvisible;
};

#endif // PICSHOW_H

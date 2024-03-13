#ifndef PICSHOW_H
#define PICSHOW_H

#include <QDialog>

namespace Ui {
class PicShow;
}

class PicShow : public QDialog
{
    Q_OBJECT

public:
    explicit PicShow(QWidget *parent = nullptr);
    ~PicShow();

private:
    Ui::PicShow *ui;
};

#endif // PICSHOW_H

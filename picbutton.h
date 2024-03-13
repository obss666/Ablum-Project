#ifndef PICBUTTON_H
#define PICBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include <QEvent>

class PicButton : public QPushButton
{
public:
    PicButton(QWidget *parent = nullptr);
    void SetIcons(const QString &normal, const QString &hover, const QString &pressed);

protected:
    bool event(QEvent *e) override;

private:
    QString _normal;
    QString _hover;
    QString _pressed;
    void setNormalIcon();
    void setHoverIcon();
    void setPressIcon();

};

#endif // PICBUTTON_H

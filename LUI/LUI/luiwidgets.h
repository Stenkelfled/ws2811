#ifndef LUICOLORLABEL_H
#define LUICOLORLABEL_H

#include <QLabel>

class LuiColorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit LuiColorLabel(QWidget *parent = 0);

signals:
    void clickedColor(QColor color);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

};

#endif // LUICOLORLABEL_H

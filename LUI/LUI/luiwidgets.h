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

class LuiColorLabelCustom : public QLabel
{
    Q_OBJECT
public:
    explicit LuiColorLabelCustom(QWidget *parent = 0);

signals:
    void clickedColor(QColor color);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

};

class LuiColorDisplay : public QLabel
{
    Q_OBJECT
public:
    explicit LuiColorDisplay(QWidget *parent = 0);
    QColor color();
    void setColor(QColor color);

private:
    QColor my_color;

};

#endif // LUICOLORLABEL_H

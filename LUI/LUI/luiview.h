#ifndef LUIVIEW_H
#define LUIVIEW_H

#include <QGraphicsView>

class LuiView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit LuiView(QWidget *parent = 0);

signals:

public slots:

protected:
    virtual void wheelEvent(QWheelEvent* event);

};

#endif // LUIVIEW_H

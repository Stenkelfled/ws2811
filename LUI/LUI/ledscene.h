#ifndef LEDSCENE_H
#define LEDSCENE_H

#include <QGraphicsScene>

class LedScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit LedScene(QObject *parent = 0);
    ~LedScene();

signals:

public slots:

};

#endif // LEDSCENE_H

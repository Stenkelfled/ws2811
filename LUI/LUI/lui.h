#ifndef LUI_H
#define LUI_H

#include <QMainWindow>
#include <QStringList>
#include <QWidget>

#include <ledscene.h>
#include <luiitem.h>
#include <serial.h>
#include <sequencescene.h>
#include <sequenceitem.h>

namespace Ui {
class Lui;
}

class Lui : public QMainWindow
{
    Q_OBJECT

public:
    explicit Lui(QWidget *parent = 0);
    ~Lui();

signals:

public slots:
    //void onDockSequenceVisibilityChanged(bool visible);

private slots:

private:
    LedScene *led_scene;
    SequenceScene *sequence_scene;

};

#endif // LUI_H

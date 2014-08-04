#ifndef LUI_H
#define LUI_H

#include <QMainWindow>
#include <QStringList>

#include "ledscene.h"
#include "serial.h"

namespace Ui {
class Lui;
}

class Lui : public QMainWindow
{
    Q_OBJECT

public:
    explicit Lui(QWidget *parent = 0);
    ~Lui();

public slots:
    void updatePortComboBox(QStringList &items);
    void enableTransmitPushButton(void);

private slots:
    void on_transmitPushButton_clicked();
    void on_actionClose_triggered();

private:
    Ui::Lui *ui;
    Serial *serial;
    LedScene *scene;

    int led_count;
};

#endif // LUI_H

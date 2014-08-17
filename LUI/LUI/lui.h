#ifndef LUI_H
#define LUI_H

#include <QMainWindow>
#include <QStringList>

#include <ledscene.h>
#include <luiitem.h>
#include <serial.h>

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
    void colorChanged(QColor color);

public slots:
    void updatePortComboBox(QStringList &items);
    void enableTransmitPushButton(void);

    void colorDisplayEnable(bool status);
    void colorDisplayChange(QColor color);

private slots:
    void on_transmitPushButton_clicked();
    void on_actionClose_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionSelectAll_triggered();

    void newLedColor(QColor color);
    void testColor(QColor color);

    void on_brightness_slider_sliderMoved(int position);
    void on_testButton_toggled(bool checked);

private:
    Ui::Lui *ui;
    Serial *serial;
    LedScene *scene;

    int led_count;
};

#endif // LUI_H

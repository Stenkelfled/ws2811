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
    void newScene();

    void on_brightness_slider_valueChanged(int position);
    void on_testButton_toggled(bool checked);

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionOpen_triggered();

private:
    Ui::Lui *ui;
    Serial *serial;
    LedScene *scene;

    qint16 led_count;
    QString file_name;
};

#endif // LUI_H

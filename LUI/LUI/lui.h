#ifndef LUI_H
#define LUI_H

#include <QMainWindow>
#include <QStringList>

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
    void colorChanged(QColor color);

public slots:
    void updatePortComboBox(QStringList &items);
    void enableTransmitPushButton(void);

    void colorDisplaySetEnabled(bool status);
    void colorDisplayEnable();
    void colorDisplayChange(QColor color);
    void sequenceDetailsSetEnabled(bool status);
    void changeCurrentSequenceItem(SequenceItem *item);

    void updateGroupLabel(QString name="");

private slots:
    void on_transmitPushButton_clicked();
    void on_actionClose_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionSelectAll_triggered();

    void newLedColor(QColor color);
    void testColor(QColor color);

    void on_brightness_slider_valueChanged(int position);
    void on_testButton_toggled(bool checked);

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionOpen_triggered();

private:
    void newScene();
    Ui::Lui *ui;
    Serial *serial;
    LedScene *led_scene;
    SequenceScene *sequence_scene;
    QLabel *group_name_label;
    SequenceItem *my_current_sequence_item;

    qint16 led_count;
    QString file_name;
};

#endif // LUI_H

#include <QComboBox>
#include <QGraphicsItem>
#include <QObject>
#include <QPalette>
#include <QtDebug>

#include <global.h>
#include <protocoll.h>
#include <utils.h>
#include "lui.h"
#include "ui_lui.h"

/*!
 * \brief Lui::Lui
 * The main window of LUI - the gread LED User Interface
 */
Lui::Lui(QWidget *parent) :
    QMainWindow(parent)
{
    led_scene = new LedScene();
    QGraphicsView *led_view = new QGraphicsView(this);
    setCentralWidget(led_view);
    led_view->setScene(led_scene);
    // ////////////////////////////////////////////////////////////////////////
    //create menu structure

    //Datei
    QMenu *menu_datei = menuBar()->addMenu("&Datei");

    //Ansicht
    QMenu *menu_ansicht = menuBar()->addMenu("&Ansicht");

}

Lui::~Lui()
{

}

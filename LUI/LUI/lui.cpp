#include <QComboBox>
#include <QGraphicsItem>
#include <QObject>
#include <QPalette>
#include <QtDebug>

#include <global.h>
#include <protocoll.h>
#include <utils.h>
#include "lui.h"
#include "luiview.h"

/*!
 * \brief Lui::Lui
 * The main window of LUI - the gread LED User Interface
 */
Lui::Lui(QWidget *parent) :
    QMainWindow(parent)
{
    this->resize(900,650);

    QGraphicsView *led_view = new LuiView(this);
    led_scene = new LedScene(led_view);
    setCentralWidget(led_view);
    led_view->setScene(led_scene);
    led_scene->fillDefault();
    // ////////////////////////////////////////////////////////////////////////
    //add DockWidgets
    QDockWidget *dock_sequence = new QDockWidget("Sequenzübersicht", this);
    QGraphicsView *sequence_view = new QGraphicsView(this);
    sequence_scene = new SequenceScene(sequence_view);
    sequence_view->setScene(sequence_scene);
    dock_sequence->setWidget(sequence_view);
    addDockWidget(Qt::BottomDockWidgetArea, dock_sequence);

    // ////////////////////////////////////////////////////////////////////////
    //create menu structure

    //Datei
    QMenu *menu_datei = menuBar()->addMenu("&Datei");

    //Ansicht -> turn off and on all DockWidgets
    QMenu *menu_ansicht = menuBar()->addMenu("&Ansicht");
    menu_ansicht->addAction(dock_sequence->toggleViewAction());

}

Lui::~Lui()
{

}



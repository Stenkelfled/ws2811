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
    //QHBoxLayout *test_layout = new QHBoxLayout(this);
    //this->setLayout(test_layout);
    QLabel *label1 = new QLabel("label1", this);
    QLabel *label2 = new QLabel("label2", this);
    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(label1);
    splitter->addWidget(label2);
}

Lui::~Lui()
{

}

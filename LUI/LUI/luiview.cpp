#include <QWheelEvent>
#include <QtDebug>

#include <luiview.h>

/**
 * @brief QGraphicsView with the ability to zoom via mouse Wheel
 * @param parent
 * This reimplements QGraphicsView and adds the functionlity to zoom,
 * pan and drop leds in the LedScene
 */
LuiView::LuiView(QWidget *parent) :
    QGraphicsView(parent)
{
    setAcceptDrops(true);
}

/**
 * @brief Adds the zooming function to LuiView
 * @param event
 */
void LuiView::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

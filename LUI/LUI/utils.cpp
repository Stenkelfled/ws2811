#include <utils.h>

namespace utils {

bool isBrightBackground(QColor color)
{
    QColor display_color = toDisplayColor(color);
    return ( (display_color.redF()*0.229 + display_color.greenF()*0.587 + display_color.blueF()*0.114) > 186.0/360.0 );
}

QColor toDisplayColor(QColor color)
{
    QColor display_color = QColor(color);
    int value = DISPLAY_HSV_VALUE(display_color.value());
    if(display_color.value() == 0){
        value = 0;
    }
    display_color.setHsv(display_color.hue(), display_color.saturation(), value);
    return display_color;
}

}


void MyQByteArray::prependUint16(QByteArray *array, const quint16 data)
{
    array->prepend( (char)((data&0xFF00)>>8) );
    array->prepend( (char)( data&0xFF ) );
}

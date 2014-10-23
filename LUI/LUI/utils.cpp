#include <utils.h>

namespace utils {

bool isBrightBackground(QColor color)
{
    return ( (color.redF()*0.229 + color.greenF()*0.587 + color.blueF()*0.114) > 186.0/360.0 );
}

}

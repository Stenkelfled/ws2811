#ifndef UTILS_H
#define UTILS_H

#include <QColor>

#define DISPLAY_HSV_VALUE(x)  ((x-1)*155/254 + 100)

namespace utils {
    bool isBrightBackground(QColor color);

    QColor toDisplayColor(QColor color);
}

#endif // UTILS_H

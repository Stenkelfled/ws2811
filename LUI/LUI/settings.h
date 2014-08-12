#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPointF>

namespace settings{
    namespace ledscene{
        constexpr QPointF invalid_pos(-9999999999,-9999999999);
    }

    namespace leditem{
        constexpr int height = 20;
        constexpr int width = 20;
        constexpr Qt::GlobalColor color = Qt::black;
        constexpr int spacing = 5;
    }

    namespace groupitem{
        constexpr int border = 10;
    }
}

#endif // SETTINGS_H

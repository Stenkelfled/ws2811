#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QPointF>

#define DISPLAY_HSV(x)  ((x-1)*155/254 + 100)

namespace settings{
    namespace ledscene{
        constexpr QPointF invalid_pos(-9999999999,-9999999999);
    }

    namespace leditem{
        constexpr int height = 20;
        constexpr int width = 20;
        constexpr Qt::GlobalColor color = Qt::black;
        constexpr int spacing = 5;
        const QString mimetype("ledmime");
    }

    namespace ledgroupitem{
        constexpr int border = 10;
        constexpr int extra_space = 20;
        constexpr Qt::GlobalColor color = Qt::lightGray;
    }

    namespace sequencegroupitem{
        constexpr int height = 40; //height of the sequence
        constexpr int space = 5; //space between two sequences
        constexpr int extra_border = 5;
        constexpr int name_text_width = 80;
        constexpr int name_text_chars = 10;
        constexpr int name_text_size = 10;
        constexpr Qt::GlobalColor background_color = Qt::lightGray;
        constexpr Qt::GlobalColor text_color = Qt::black;
    }

    namespace sequenceitem {
        constexpr int pixels_per_10ms = 1;
        constexpr int height = sequencegroupitem::height - 2*sequencegroupitem::extra_border;
        constexpr int space = 5; //vertical space between two sequenceitems
    }

}

#endif // SETTINGS_H

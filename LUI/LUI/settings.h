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
        constexpr int space = 5; //space(x) between two sequences
        constexpr int group_space = 5; //vertical(y) space between two groups
        constexpr int extra_border = 5;
        constexpr int name_text_width = 80;
        constexpr int name_text_chars = 10;
        constexpr int name_text_size = 10;
        constexpr Qt::GlobalColor background_color = Qt::lightGray;
        constexpr Qt::GlobalColor text_color = Qt::black;
    }

    namespace sequencecursoritem{
        constexpr int additional_height = 5;
        constexpr int height_per_group = sequencegroupitem::height + additional_height;
        constexpr int initial_height = sequencegroupitem::height + 2*additional_height;
        constexpr int width = 2;
        constexpr Qt::GlobalColor dark_color = Qt::black;
        constexpr Qt::GlobalColor bright_color = Qt::white;
        //constexpr Qt::GlobalColor color = dark_color;
    }

    namespace sequenceitem {
        constexpr int pixels_per_10ms = 1;
        constexpr int height = sequencegroupitem::height - 2*sequencegroupitem::extra_border;
        constexpr int space = 5; //vertical space between two sequenceitems
    }

}

namespace usertype{
    constexpr int luiitem = 1;
    constexpr int ledgroupitem = 2;
    constexpr int leditem = 3;
    constexpr int sequencegroupitem = 4;
    constexpr int sequenceitem = 5;
    constexpr int sequencecursoritem = 6;
}

#endif // SETTINGS_H

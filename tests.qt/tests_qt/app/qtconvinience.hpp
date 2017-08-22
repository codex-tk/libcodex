#ifndef QTCONVINIENCE_HPP
#define QTCONVINIENCE_HPP

#include <QtWidgets/QLabel>
#include <codex/vision/image.hpp>

class QTConvinience
{
public:
    QTConvinience();

    static void bind( QLabel* lb , codex::vision::image& img );
};

#endif // QTCONVINIENCE_HPP

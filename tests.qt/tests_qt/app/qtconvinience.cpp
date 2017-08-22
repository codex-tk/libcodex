#include "qtconvinience.hpp"

QTConvinience::QTConvinience()
{

}

void QTConvinience::bind( QLabel* lb , codex::vision::image& img ){
    QImage qimg(img.ptr()
                , img.width()
                , img.height() , QImage::Format_Grayscale8 );
    lb->setPixmap( QPixmap::fromImage(qimg));
    lb->setScaledContents( true );
    lb->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

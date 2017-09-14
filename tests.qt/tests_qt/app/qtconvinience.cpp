#include "qtconvinience.hpp"

QTConvinience::QTConvinience()
{

}

void QTConvinience::bind( QLabel* lb , codex::vision::image& img ){
    QImage::Format fmt;
    if ( img.channel() == 1 ){
        fmt = QImage::Format_Grayscale8;
    } else if ( img.channel() == 3 ){
        fmt = QImage::Format_RGB888;
    } else if ( img.channel() == 4 ){
        fmt = QImage::Format_ARGB32;
    }
    QImage qimg(img.ptr()
                , img.width()
                , img.height() , fmt );
    lb->setPixmap( QPixmap::fromImage(qimg));
    lb->setScaledContents( true );
    lb->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

/*
void CustomLabel::mousePressEvent( QMouseEvent* ev )
{
    const QPoint p = ev->pos();
    emit mousePressed( p );
}

CustomLabel::CustomLabel( QWidget * parent, Qt::WindowFlags f )
    : QLabel( parent, f ) {}

CustomLabel::CustomLabel( const QString& text, QWidget* parent, Qt::WindowFlags f )
    : QLabel( text, parent, f ) {}
*/
